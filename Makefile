# SPDX-License-Identifier: GPL-3.0
MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

abs_root := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
abs_build := $(abs_root)/build

DOTCONFIG :=  $(abs_root)/.config
HOST := linux
ARCH := $(shell uname -m)

export DOTCONFIG HOST ARCH

all_targets := cmake_config cmake_build menuconfig clean distclean

.PHONY: $(all_targets)

cmake_build: cmake_config
	@cmake --build $(abs_build)

cmake_config:
	@cmake $(abs_root) -B $(abs_build)

menuconfig:
	@MENUCONFIG_STYLE=aquatic menuconfig

clean:
	@make -C $(abs_build) clean

distclean:
	@rm -r $(abs_build)/*
