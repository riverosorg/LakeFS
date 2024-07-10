# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# Here to make things easier when unfamiliar with Meson

BUILD_DIR := build

all: build

.PHONY: setup
setup:
	meson setup $(BUILD_DIR)

.PHONY: build
build:
	meson compile -C $(BUILD_DIR)

.PHONY: test
test:
	meson test -C $(BUILD_DIR)

.PHONY: run_fs
run_fs: build
	-fusermount -u /lakefs
	$(BUILD_DIR)/filesystem/lakefs

.PHONY: run_cli
run_cli: build
	$(BUILD_DIR)/cli/lakefs-cli