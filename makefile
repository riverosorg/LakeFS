# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# Here to make things easier when unfamiliar with Meson

BUILD_DIR ?= build

.PHONY: help
help: 
	@echo "make [BUILD_DIR=] <COMMAND>"
	@echo ""
	@echo "Optional Arguments:"
	@echo "  BUILD_DIR - Sets a custom location for the meson build directory. Default is 'build'"
	@echo ""
	@echo "Commands:"
	@echo "  setup      - Creates the meson build directoy. Must be run once before being able to build the project"
	@echo "  build      - Builds the project"
	@echo "  install    - Runs mesons install command. Will overwrite /etc/lakefs.conf"
	@echo "  test       - Runs all project tests"
	@echo "  local-test - Runs project tests, but uses unshare to create a seperate cgroup for lakefs. Helps mitigate issues while testing due to fuse locking the filesystem during a failure in lakefs"
	@echo "  clean      - Cleans mesons build directory"

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

.PHONY: clean
clean:
	meson compile --clean -C $(BUILD_DIR)