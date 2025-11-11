// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <cstring>
#include <csignal>
#include <memory>
#include <thread>
#include <unordered_map>
#include <filesystem>

#include <argparse/argparse.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/logger.h>
// #include <spdlog/common.h>

extern "C" { 
    #include <unistd.h> 
}

#include <sqlite3.h>

#include "db.hpp"
#include "fs.hpp"
#include "control.hpp"
#include "config.hpp"
#include "backups.hpp"
#include "metadata.h"

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

    program.add_argument("default_query")
        .default_value("default")
        .help("Specify the initial default query of the mount");

    program.add_argument("--tempdb")
        .flag()
        .help("Use an in-memory database instead of the file");

    program.add_argument("--config", "-c")
        .default_value("/etc/lakefs.conf")
        .help("Manually set a config location rather than using /etc/lakefs.conf");

    program.add_argument("-f")
        .flag()
        .help("Run program in foreground rather than as a daemon");

    program.add_argument("-d")
        .flag()
        .help("Output complete debug information while running");

    try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
        spdlog::critical("Error collecting arguments: {0}", err.what());

		return 1;
	}

    const auto is_debug = program.get<bool>("-d");

    // Get our configuration
    const auto config_path = std::filesystem::absolute(
        program.get<std::string>("--config")
    );

    auto config = etc_conf_reader(config_path);

    if (config.empty() && !program.get<bool>("--tempdb")) {
        spdlog::error("Failed to read configuration file at {0}", config_path.string());
        return 1;
    }

    // Extract mount point
    const auto mount_point = std::filesystem::absolute(
        program.get<std::string>("mount_point")
    );

    // Extract default query
    const auto default_query = program.get<std::string>("default_query");
    db_set_default_query(default_query);

    // Initialize file logger
    if (!program.get<bool>("--tempdb"))
    {
        const std::string log_file_name = "/var/lakefs/lakefs.log";
        
        // create a rotating logger with a 5MB file size and three files
        const auto max_size = 1048576 * 5;
        const auto max_files = 3;
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file_name, max_size, max_files);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    
        spdlog::logger logger("default", {console_sink, rotating_sink});
        spdlog::default_logger()->swap(logger);
    }

    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);

    // NOTE argv[0] is program name in C!! and is ignored by fuse
    fuse_opt_add_arg(&args, "lakefs");

    if (is_debug) {
        spdlog::info("Running in debug mode");

        spdlog::set_level(spdlog::level::trace);
        fuse_opt_add_arg(&args, "-d");

    } else {
        const auto log_level = config["log_level"];

        spdlog::set_level(spdlog::level::from_str(log_level));
    }

    spdlog::trace("Initializing LakeFS");

    // Do not fork fusemain on launch
    fuse_opt_add_arg(&args, "-f");

    // Set FS permissions to default
    fuse_opt_add_arg(&args, "-odefault_permissions");

    // mount point
    spdlog::info("Mounting at {0}", mount_point.string());
    fuse_opt_add_arg(&args, mount_point.c_str());

    // Initialize SQLLite
    int rc;

    if (program.get<bool>("--tempdb")) {
        spdlog::info("Using in-memory database");
        
        rc = db_tmp_init();
    } else {
        rc = db_init(config["dir"]);

        const auto interval = parse_interval_value(config["backup_interval"]);

        if (!interval.has_value()) {
            spdlog::warn("backup_interval malformed. Continuing without backups");
            
        } else {
            // Create backup timer
            if (create_backup_timer(interval.value(), std::stoi(config["max_backups"]), config["dir"])) {
                spdlog::info("Backup system started");
            
            } else {
                spdlog::warn("Could not start backup system. Continuing without backups");
            }
        }
    }

    if (rc != SQLITE_OK) {
        spdlog::critical("Failed to initialize SQLite3: {0}", rc);
        return 1;
    }

    const auto is_daemon = !program.get<bool>("-f");

    if (is_daemon) {
        spdlog::trace("Launching as daemon");

        // Forks program and runs in background
        const auto rc = daemon(0, 1);

        if (rc < 0) {
            spdlog::critical("Error starting daemon: {0}", strerror(errno));
        }
    }

    // Initialize the control server
    std::thread control_thread(control_server);

    int ret = fuse_main(args.argc, args.argv, &operations, nullptr);

    fuse_opt_free_args(&args);

    return ret;
}