# SPDX-License-Identifier: GPL-3.0
abs_root := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
abs_build := $(abs_root)/build

ROOT := $(abs_root)
BUILD := $(abs_build)
DOTCONFIG :=  $(abs_root)/.config

export ROOT BUILD DOTCONFIG

all_targets := all menuconfig clean distclean

.PHONY: $(all_targets)

all:
	@cmake $(abs_root) -B $(abs_build)

menuconfig:
	@MENUCONFIG_STYLE=aquatic menuconfig

clean:
	@make -C $(abs_build) clean

distclean:
	@rm -r $(abs_build)/*
