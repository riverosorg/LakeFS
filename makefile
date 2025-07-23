# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# Here to make things easier when unfamiliar with Meson

BUILD_DIR := build

all: build

.PHONY: setup
setup:
	meson setup $(BUILD_DIR)

.PHONY: build 
build: build-fs 
	meson compile -C $(BUILD_DIR)	

.PHONY: build-fs
build-fs:
	cd filesystem && cargo build
	mkdir -p $(BUILD_DIR)/filesystem
	mv filesystem/target/debug/lakefs $(BUILD_DIR)/filesystem

.PHONY: install
install:
	meson install -C $(BUILD_DIR)
	cp lakefs.conf /etc/lakefs.conf

.PHONY: test
test: build
	meson test -C $(BUILD_DIR) --print-errorlogs

# Local test runs unshare to segment the resources and limit the chance of locking up
.PHONY: local-test
local-test:
	unshare -pfr --user --mount --kill-child meson test -C $(BUILD_DIR) --print-errorlogs

.PHONY: clean
clean:
	meson compile --clean -C $(BUILD_DIR)