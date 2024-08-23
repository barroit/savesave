# SPDX-License-Identifier: GPL-3.0-only
MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

abs_root  := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
abs_build := $(abs_root)/build
abs_gen   := $(abs_root)/include/generated

export ROOT := $(abs_root)

DOTCONFIG := $(abs_root)/.config
AUTOCONF  := $(abs_gen)/autoconf.h
export DOTCONFIG AUTOCONF

BUILD := linux
ARCH := $(shell uname -m)
export BUILD ARCH

all_targets := build cmake_config cmake_prepare menuconfig clean distclean

.PHONY: $(all_targets)

build: cmake_config
	@cmake --build $(abs_build) --parallel

cmake_config: cmake_prepare
	@cmake $(abs_root) -B $(abs_build)

cmake_prepare:
	@mkdir -p $(abs_gen)
	@genconfig --header-path $(AUTOCONF)

menuconfig:
	@MENUCONFIG_STYLE=aquatic menuconfig

clean:
	@make -C $(abs_build) clean

distclean:
	@rm -f $(abs_gen)/*
	@rm -rf $(abs_build)/*
	@rm -f $(abs_root)/.config*
