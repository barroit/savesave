// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Implementation based on the example provided by liburing and
 * https://man7.org/linux/man-pages/man7/io_uring.7.html
 *
 * Unfortunately, this io_uring does not speed up file copying. It even slows
 * the speed down compared to file_copy_range(2).
 */

#include "acpy.h"
#include "termas.h"
#include "fileiter.h"
#include "strbuf.h"
#include "mkdir.h"
#include "fcpy.h"
#include "path.h"
#include "alloc.h"
#include "list.h"

#define MAX_ENTRIES INT16_MAX

#define URING_MMAP_PROT PROT_READ | PROT_WRITE
#define URING_MMAP_FLAG MAP_SHARED | MAP_POPULATE

#define ERRMAS_ACPY_LOWMEM \
	_("machine is running critically low on memory, copy stopped")

#define is_ring_heavy_load (acpy.entries > acpy.msub)

enum acpy_ud_status {
	ACPY_READ,
	ACPY_WRITE,
};

struct acpy_ud {
	uint fd_idx;	/* current fd index (read/write) */

	int fd[2];	/* file descriptors input and output, order is the same
			   as pipe(2). NB: fd[0] and fd[1] maybe shared across
			   user data */
	u32 *fd_ref;	/* fd_ref[0] and fd_ref[1] are usage count of these two
			   fds, and fd_ref[2] is set to 1 to indicate the
			   request submission of current file is done */

	void *buf;	/* buffer for both read and write */
	u32 len;	/* number of bytes to read/write */
	u64 off;	/* file offset */
};

struct acpy_sq {
	void *base;		/* first address of SQ */

	/*
	 * the submission side has an indirection array between them. The
	 * submission side ring buffer is an index into this array.
	 */
	u32 *head;	/* head of SQ */
	u32 *tail;	/* tail of SQ */
	u32 mask;	/* index mask */
	u32 *flags;	/* flags */
};

struct acpy_cq {
	void *base;

	/*
	 * the CQ ring directly indexes the shared array of CQEs
	 */
	u32 *head;	/* head of CQ */
	u32 *tail;	/* tail of CQ */
	u32 mask;	/* index mask of CQ */
};

struct acpy {
	int ring_fd;			/* file descriptor of uring */

	struct acpy_sq sq;		/* submission queue */
	struct acpy_cq cq;		/* completion queue */

	u32 entries;			/* number of entries that have been
					   submitted but not yet processed */

	struct io_uring_sqe *sqes;	/* submission queue entries */
	struct io_uring_cqe *cqes;	/* completion queue entries */

	uint sqsz;			/* sq size */
	uint msub;			/* max submission */
	uint mrem;			/* max remaining */
	size_t bs;			/* block size */
};

static struct acpy acpy;

int acpy_disabled;

/*
 * A wrapper to io_uring_setup(2).
 */
static int io_uring_setup(u32 entries, struct io_uring_params *p)
{
	return __syscall(__NR_io_uring_setup, entries, p);
}

/*
 * A wrapper to io_uring_enter(2). This function handles interrupts
 * automatically.
 */
static int io_uring_enter(int fd, u32 to_submit, u32 min_complete, u32 flags)
{
	int ret;

retry:
	ret = __syscall(__NR_io_uring_enter,
			fd, to_submit, min_complete, flags, NULL, 0);

	if (unlikely(ret == -EINTR))
		goto retry;

	return ret;
}

static void map_addr(typeof_member(struct io_uring_params, sq_off) *so,
		     typeof_member(struct io_uring_params, cq_off) *co)
{
	acpy.sq.head = acpy.sq.base + so->head;
	acpy.sq.tail = acpy.sq.base + so->tail;
	acpy.sq.mask = *(u32 *)(acpy.sq.base + so->ring_mask);
	acpy.sq.flags = acpy.sq.base + so->flags;

	acpy.cq.base = acpy.sq.base;
	acpy.cqes = acpy.cq.base + co->cqes;

	acpy.cq.head = acpy.cq.base + co->head;
	acpy.cq.tail = acpy.cq.base + co->tail;
	acpy.cq.mask = *(u32 *)(acpy.cq.base + co->ring_mask);
}

static void assert_features(typeof_member(struct io_uring_params, features) f)
{
	/*
	 * The minimum kernel version savesave runs on is 5.19. These feature
	 * should exist.
	 */
	BUG_ON(!(f & IORING_FEAT_SINGLE_MMAP));
	BUG_ON(!(f & IORING_FEAT_NODROP));
	BUG_ON(!(f & IORING_FEAT_SQPOLL_NONFIXED));
}

void acpy_deploy(uint qs, size_t bs)
{
	if (qs > INT16_MAX) {
		warn(_("qs is too large, clamp to %u)"), MAX_ENTRIES);
		qs = MAX_ENTRIES;
	}

	if (bs > MAX_ALLOC_SIZE) {
		warn(_("bs is too large, clamp to %zu)"), MAX_ALLOC_SIZE);
		bs = MAX_ALLOC_SIZE;
	}

	struct io_uring_params p = {
		.sq_entries = qs,
		.cq_entries = p.sq_entries << 1,
		.flags      = IORING_SETUP_SQPOLL | IORING_SETUP_CQSIZE,
	};

	acpy.sqsz = qs;
	acpy.msub = (qs >> 1) * 2;
	acpy.mrem = (qs >> 1) * 1;
	acpy.bs = bs;

	/*
	 * read queue depth from '/sys/block/nvme0n1/queue/nr_requests'?
	 */
	acpy.ring_fd = io_uring_setup(p.sq_entries, &p);
	if (acpy.ring_fd < 0) {
		errno = -acpy.ring_fd;
		warn_errno(_("failed to setup io uring"));
		goto err;
	}
	assert_features(p.features);

	size_t size = p.sq_off.array + p.sq_entries * sizeof(*acpy.sq.head);
	acpy.sq.base = mmap(NULL, size, URING_MMAP_PROT, URING_MMAP_FLAG,
			    acpy.ring_fd, IORING_OFF_SQ_RING);

	if (acpy.sq.base == MAP_FAILED) {
		warn_errno(_("failed to allocate submission queue"));
		goto err;
	}

	size_t i;
	u32 *sqarr = acpy.sq.base + p.sq_off.array;
	for_each_idx(i, p.sq_entries)
		sqarr[i] = i;

	size = p.sq_entries * sizeof(*acpy.sqes);
	acpy.sqes = mmap(NULL, size, URING_MMAP_PROT, URING_MMAP_FLAG,
			 acpy.ring_fd, IORING_OFF_SQES);

	if (acpy.sqes == MAP_FAILED) {
		warn_errno(_("failed to allocate submission queue entries"));
		goto err;
	}

	map_addr(&p.sq_off, &p.cq_off);
	return;

err:
	acpy_disabled = 1;
	warn(_("asynchronous io is disabled"));
}

/*
 * Peek a CQE from CQ
 * Return -EAGAIN if CQ is empty.
 */
static int peekcqe(struct io_uring_cqe **cqe)
{
	u32 tail = __atomic_load_n(acpy.cq.tail, __ATOMIC_ACQUIRE);
	u32 head = *acpy.cq.head;

	if (tail == head)
		return -EAGAIN;

	*cqe = &acpy.cqes[head & acpy.cq.mask];
	return 0;
}

/*
 * Wait a CQE to complete.
 * On error, -EBADR is returned. In this case, the program should terminate.
 */
static int waitcqe(struct io_uring_cqe **cqe)
{
	int err = peekcqe(cqe);
	if (!err)
		return 0;

	/*
	 * Impossible return value in this path (until 6.11)
	 *	EAGAIN	EBADF	EBADFD		EEXIST		EINVAL
	 *	EFAULT	ENXIO	EOPNOTSUPP	EINTR (handled)
	 *
	 * EBADR - call exit(3)
	 * EBUSY - not seen in source
	 */
	err = io_uring_enter(acpy.ring_fd, 0, 1, IORING_ENTER_GETEVENTS);
	if (err)
		return err;

	err = peekcqe(cqe);
	BUG_ON(err);

	return 0;
}

static void compcqe(void)
{
	u32 head = *acpy.cq.head;
	head += 1;
	__atomic_store_n(acpy.cq.head, head, __ATOMIC_RELEASE);
}

static struct io_uring_sqe *acquire_sqe(void)
{
	u32 head = __atomic_load_n(acpy.sq.head, __ATOMIC_ACQUIRE);
	u32 tail = *acpy.sq.tail;
	u32 new = tail & acpy.sq.mask;

	if (tail + 1 == head)
		return NULL;

	return &acpy.sqes[new];
}

static void push_sqe(void)
{
	u32 tail = *acpy.sq.tail;

	tail += 1;
	__atomic_store_n(acpy.sq.tail, tail, __ATOMIC_RELEASE);
}

static void describe_sqe(struct io_uring_sqe *sqe, struct acpy_ud *ud)
{
	sqe->fd = ud->fd[ud->fd_idx];

	if (ud->fd_idx == ACPY_READ)
		sqe->opcode = IORING_OP_READ;
	else
		sqe->opcode = IORING_OP_WRITE;

	sqe->addr = (typeof(sqe->addr))ud->buf;
	sqe->len = ud->len;
	sqe->off = ud->off;

	sqe->user_data = (typeof(sqe->user_data))ud;
}

static void inform_sq(void)
{
	/*
	 * We need a full memory barrier before checking for
	 * IORING_SQ_NEED_WAKEUP after updating the sq tail
	 */
	__atomic_thread_fence(__ATOMIC_SEQ_CST);

	u32 flags = __atomic_load_n(acpy.sq.flags, __ATOMIC_RELAXED);
	if (flags & IORING_SQ_NEED_WAKEUP)
		/*
		 * No error in this path
		 */
		io_uring_enter(acpy.ring_fd, 0, 0, IORING_ENTER_SQ_WAKEUP);
}

/*
 * Submit a asynchronous io request.
 * Return -EBUSY if there's no room to make this request.
 */
static int submit_request(int *fd, u32 len, u64 off, u32 *fd_usage)
{
	if (is_ring_heavy_load)
		return -EBUSY;

	struct io_uring_sqe *sqe = acquire_sqe();
	if (!sqe)
		return -EBUSY;

	struct acpy_ud *ud = calloc(sizeof(*ud), 1);
	if (!ud)
		return -EBUSY;

	void *buf = malloc(len);
	if (!buf) {
		free(ud);
		return -EBUSY;
	}

	ud->fd[0] = fd[0];
	ud->fd[1] = fd[1];

	ud->fd_ref = fd_usage;
	ud->fd_ref[ud->fd_idx]++;

	ud->buf = buf;
	ud->len = len;
	ud->off = off;

	describe_sqe(sqe, ud);

	push_sqe();
	acpy.entries++;

	inform_sq();
	return 0;
}

static void resubmit_request(struct acpy_ud *ud)
{
	struct io_uring_sqe *sqe = acquire_sqe();
	BUG_ON(!sqe);

	describe_sqe(sqe, ud);
	ud->fd_ref[ud->fd_idx]++;

	push_sqe();
	inform_sq();
}

static void drop_user_data(struct acpy_ud *ud)
{
	ud->fd_ref[ud->fd_idx]--;
	acpy.entries--;

	if (unlikely(!ud->fd_ref[0] && !ud->fd_ref[1] && ud->fd_ref[2])) {
		close(ud->fd[0]);
		close(ud->fd[1]);
		free(ud->fd_ref);
	}

	free(ud->buf);
	free(ud);
}

static int handle_cqe_error(struct io_uring_cqe *cqe)
{
	int err;
	struct acpy_ud *ud = (typeof(ud))cqe->user_data;
	char *name;
	int fd = ud->fd_idx == ACPY_READ ? ud->fd[0] : ud->fd[1];
	struct strbuf sb = STRBUF_INIT;

	strbuf_printf(&sb, "/proc/self/fd/%d", fd);
	err = READLINK(sb.str, &name);
	if (err)
		goto free_strbuf;

	errno = -cqe->res;
	if (ud->fd_idx == ACPY_READ)
		warn_errno(ERRMAS_READ_FILE(name));
	else
		warn_errno(ERRMAS_WRITE_FILE(name));

	free(name);

free_strbuf:
	strbuf_destroy(&sb);

	if (err) {
		errno = -cqe->res;
		if (ud->fd_idx == ACPY_READ)
			warn_errno(_("failed to read fd `%d'"), fd);
		else
			warn_errno(_("failed to write to fd `%d'"), fd);
	}

	drop_user_data(ud);
	return -1;
}

static int process_result(void)
{
	int err;
	struct io_uring_cqe *cqe;

again:
	err = waitcqe(&cqe);
	if (err)
		return warn(ERRMAS_ACPY_LOWMEM);

	do {
		if (unlikely(cqe->res < 0)) {
			acpy.entries--;
			return handle_cqe_error(cqe);
		}

		struct acpy_ud *ud = (typeof(ud))cqe->user_data;

		if (unlikely(cqe->res < ud->len)) {
			ud->off += cqe->res;
			ud->len -= cqe->res;
			resubmit_request(ud);
		} else if (ud->fd_idx == ACPY_READ) {
			ud->fd_ref[ud->fd_idx]--;
			ud->fd_idx = ACPY_WRITE;
			resubmit_request(ud);
		} else {
			drop_user_data(ud);
		}

		compcqe();
	} while (!(err = peekcqe(&cqe)));

	if (acpy.entries >= acpy.mrem)
		goto again;

	return 0;
}

static void drop_entries(void)
{
	struct io_uring_cqe *cqe;
	int err;

again:
	err = waitcqe(&cqe);
	if (err)
		die(ERRMAS_ACPY_LOWMEM);

	do {
		struct acpy_ud *ud = (typeof(ud))cqe->user_data;

		drop_user_data(ud);
	} while (!(err = peekcqe(&cqe)));

	if (acpy.entries)
		goto again;
}

static int retrieve_file_info(const char *src,
			      const char *dest, int *fd, struct stat *st)
{
	int in = open(src, O_RDONLY);
	if (in == -1) {
		warn_errno(ERRMAS_OPEN_FILE(src));
		goto err_out;
	}

	int out = flexcreat(dest);
	if (out == -1) {
		warn_errno(ERRMAS_OPEN_FILE(dest));
		goto err_open_dest;
	}

	int err = fstat(in, st);
	if (err) {
		warn_errno(ERRMAS_STAT_FILE(src));
		goto err_stat_src;
	}

	fd[0] = in;
	fd[1] = out;
	return 0;

err_stat_src:
	close(out);
err_open_dest:
	close(in);
err_out:
	return -1;
}

static int release_heavy_load(void)
{
	int err = process_result();
	if (!err)
		return 0;

	drop_entries();
	return err;
}

static int acpyreg(const char *srcname, const char *destname)
{
	int err;

	if (is_ring_heavy_load) {
		err = release_heavy_load();
		if (err)
			return err;
	}

	int fd[2];
	struct stat st;

	err = retrieve_file_info(srcname, destname, fd, &st);
	if (err)
		return err;

	u32 len = 0;
	u64 off = 0;
	off_t remain = st.st_size;
	u32 *fd_usage = xcalloc(sizeof(*fd_usage), 3);

again:
	off += len;

	if (remain <= acpy.bs) {
		len = remain;
		remain = 0;
	} else {
		len = acpy.bs;
		remain -= acpy.bs;
	}

retry:
	err = submit_request(fd, len, off, fd_usage);
	if (unlikely(err)) {
		err = release_heavy_load();
		if (err) {
			free(fd_usage);
			return err;
		}
		goto retry;
	}

	if (unlikely(remain))
		goto again;

	fd_usage[2] = 1;
	return 0;
}

static int cp_reg_and_unsuppd(struct iterfile *src, void *data)
{
	struct strbuf *dest = data;
	strbuf_concatat_base(dest, src->dymname);

	return acpyreg(src->absname, dest->str);
}

int acpy_copy(const char *src, const char *dest)
{
	int ret;
	struct strbuf data = strbuf_from2(dest, 0, PATH_MAX);

	ret = fileiter(src, __fiter_cpy_nonreg, &data, __FITER_CPY_NONREG);
	if (ret)
		goto err_cpy;

	ret = fileiter(src, cp_reg_and_unsuppd, &data, FITER_NO_SYMLINK);
	if (ret)
		goto err_cpy;

	while (acpy.entries && !ret)
		ret = process_result();

	if (ret) {
	err_cpy:
		warn(ERRMAS_COPY_FILE(src, dest));
	}

	strbuf_destroy(&data);
	return ret;
}
