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

docs/%.1: docs/%.rst
	rst2man $< $@

docs/%.1.gz: docs/%.1
	gzip $<

.PHONY: docs
docs: docs/lakefs-cli.1.gz docs/lakefs.1.gz docs/lakefs.conf.1.gz
	-mkdir docs/man1
	mv docs/*.1.gz docs/man1

.PHONY: clean
clean:
	meson compile --clean -C $(BUILD_DIR)