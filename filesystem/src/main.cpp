// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <iostream>
#include <thread>
#include <unordered_map>
#include <fstream>

#include <argparse/argparse.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "sqlite/sqlite3.h"

#include "db.hpp"
#include "fs.hpp"
#include "control.hpp"
#include "config.h"

auto etc_conf_reader(std::string path) -> std::unordered_map<std::string, std::string>;

static const struct fuse_operations operations = {
    .getattr  = lake_getattr,
    .readlink = nullptr,
    .mknod    = nullptr,
    .mkdir    = nullptr,
    .unlink   = nullptr,
    .rmdir    = nullptr,
    .symlink  = nullptr,
    .rename   = nullptr,
    .link     = nullptr,
    .chmod    = nullptr,
    .chown    = nullptr,
    .truncate = nullptr,
    .open     = lake_open,
    .read     = lake_read,
    .write    = lake_write,
    .statfs   = nullptr,
    .release  = lake_release,
    .fsync    = nullptr,
    .readdir  = lake_readdir,
    .destroy  = lake_destroy,
    .access   = nullptr,
    .create   = nullptr,
    .ioctl    = nullptr,
};

auto main(int argc, char** argv) -> int {
    // Set up the CLI args

    argparse::ArgumentParser program("lakefs", LAKEFS_VERSION);
    program.add_description("LakeFS - A tag based abstraction over the filesystem");

    program.add_argument("mount_point")
        .required()
        .help("The directory to mount the filesystem at");

    program.add_argument("--tempdb")
        .flag()
        .help("Use an in-memory database instead of the file");

    program.add_argument("--config", "-c")
        .default_value("/etc/lakefs.conf")
        .help("Manually set a config location rather than using /etc/lakefs.conf");

    try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		exit(1);
	}

    // Get our configuration
    const auto config_path = program.get<std::string>("--config");

    auto config = etc_conf_reader(config_path);

    if (config.empty() && !program.get<bool>("--tempdb")) {
        spdlog::error("Failed to read configuration file at {0}", config_path);
        return 1;
    }

    // Extract mount point
    mount_point = program.get<std::string>("mount_point");

    // Initialize file logger
    // auto file_logger = spdlog::basic_logger_mt("file_logger", "lakefs.log");
    // spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::trace);

    spdlog::trace("Initializing LakeFS");
    
    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);
    
    // run in foreground
    fuse_opt_add_arg(&args, "-f");
    fuse_opt_add_arg(&args, "-d");
    // TODO: We rely on foreground running for our control thread.
    // We may want to fork our own process and run in the background
    // Giving the caller behaviour expected of launching a

    // 
    fuse_opt_add_arg(&args, "-odefault_permissions");

    // mount point
    spdlog::info("Mounting at {0}", mount_point);
    fuse_opt_add_arg(&args, mount_point.c_str());

    // Initialize SQLLite
    int rc;

    if (program.get<bool>("--tempdb")) {
        spdlog::info("Using in-memory database");
        
        rc = db_tmp_init();
    } else {
        rc = db_init(config["dir"]);
    }

    if (rc != SQLITE_OK) {
        spdlog::critical("Failed to initialize SQLite3: {0}", rc);
        return 1;
    }

    // Initialize the control server
    std::thread control_thread(control_server);

    int ret = fuse_main(args.argc, args.argv, &operations, nullptr);

    fuse_opt_free_args(&args);

    return ret;
}

auto etc_conf_reader(std::string path) -> std::unordered_map<std::string, std::string> {
    std::unordered_map<std::string, std::string> config;

    std::ifstream file(path);

    if (!file.is_open()) {
        spdlog::error("Failed to open {0}", path);
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '#') {
            continue;
        }

        std::string key;
        std::string value;

        std::istringstream line_stream(line);
        std::getline(line_stream, key, '=');
        std::getline(line_stream, value);

        config[key] = value;

        spdlog::debug("Config line: {0}={1}", key, value);
    }

    return config;
}