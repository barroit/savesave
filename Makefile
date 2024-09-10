# SPDX-License-Identifier: GPL-3.0-only
MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

abs_root  := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
abs_build := $(abs_root)/build

BUILD := linux
ARCH  := $(shell uname -m)
SAVESAVE_VERSION := $(shell cat $(abs_root)/version)
export BUILD ARCH SAVESAVE_VERSION

.PHONY: build clean distclean

build:
	@cmake -S $(abs_root) -B $(abs_build)
	@cmake --build $(abs_build) --parallel

clean:
	@make -C $(abs_build) clean

distclean:
	@rm -rf $(abs_root)/include/generated
	@git ls-files --directory -o $(abs_root)/build | xargs rm -rf
	@rm -f $(abs_root)/.config*

.PHONY: menuconfig

menuconfig:
	@MENUCONFIG_STYLE=aquatic menuconfig

scripts := $(notdir $(wildcard scripts/*.sh))
args    := $(filter-out $(scripts),$(MAKECMDGOALS))

.PHONY: $(args)

$(args):
	@:

.PHONY: $(scripts)

$(scripts):
	@bash scripts/$@ $(args)
