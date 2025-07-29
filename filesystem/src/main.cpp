// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <chrono>
#include <cstring>
#include <ctime>
#include <csignal>
#include <iostream>
#include <sys/_clock_id.h>
#include <sys/signal.h>
#include <thread>
#include <unordered_map>
#include <filesystem>
#include <optional>

#include <time.h>
#include <signal.h>

#include <argparse/argparse.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

extern "C" { 
    #include <unistd.h> 
}

#include "sqlite/sqlite3.h"

#include "db.hpp"
#include "fs.hpp"
#include "control.hpp"
#include "config.hpp"
#include "metadata.h"

static auto create_backup_timer(std::chrono::seconds interval, uint32_t number_backups) -> void;

// better way to pass these in? handler args
uint32_t _number_backups;
std::string _backup_dir;

static auto handle_backup(sigval val) -> void;

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

    program.add_argument("-f")
        .flag()
        .help("Run program in foreground rather than as a daemon");

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

    // Do not fork fusemain on launch
    fuse_opt_add_arg(&args, "-f");

    // Debug info
    fuse_opt_add_arg(&args, "-d");

    // Set FS permissions to default
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

        const auto interval = parse_interval_value(config["backup_interval"]);

        // Create backup timer
        _backup_dir = config["dir"];
        _number_backups = std::stoi(config["max_backups"]);
        
        create_backup_timer(interval, std::stoi(config["max_backups"]));

        return 0;
    }

    if (rc != SQLITE_OK) {
        spdlog::critical("Failed to initialize SQLite3: {0}", rc);
        return 1;
    }

    const auto is_daemon = !program.get<bool>("-f");

    if (is_daemon) {
        spdlog::trace("Launching as daemon");

        // Forks program and runs in background
        const auto rc = daemon(1, 1);

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

static auto create_backup_timer(std::chrono::seconds interval, uint32_t number_backups) -> void {
    spdlog::info("Setting up backups to keep {0} copies and run every {1} hours", 
        number_backups, 
        std::chrono::duration_cast<std::chrono::hours>(interval).count());

    _number_backups = number_backups;

    // Create a C timer    
    sigevent event;

    // Will spawn signal handler in its own thread
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = &handle_backup;
    event.sigev_notify_attributes = NULL;

    timer_t timer_id;
    if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1) {
        spdlog::critical("Could not create timer! {0}", strerror(errno));
        // todo: pass up
    }

    itimerspec timer_spec;
    timer_spec.it_interval.tv_sec = interval.count();

    //start timer
    if (timer_settime(timer_id, 0, &timer_spec, NULL)) {
        spdlog::critical("Could not start timer! {0}", strerror(errno));
        // todo: pass up
    }
}

static auto handle_backup(sigval val) -> void {
    spdlog::info("Starting Backup...");

    const auto now = time(0);
    const auto current_time = *std::localtime(&now);
    char buf [128];

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &current_time);

    std::string backup_path = buf;
    backup_path += ".backup.db";

    db_create_backup(_backup_dir + "/" + backup_path);

    spdlog::info("DB Backup file created at {0}", _backup_dir + "/" + backup_path);

    // delete files if needed..
    auto dir_iter = std::filesystem::directory_iterator(_backup_dir);

    int backup_count = std::count_if(
        begin(dir_iter),
        end(dir_iter),
        [](auto& entry) { 
            const auto filename = entry.path().filename();

            return entry.is_regular_file() && (filename.string().ends_with(".backup.db"));
        }
    );

    if (backup_count > _number_backups) {
        spdlog::info("Removing oldest backup");

        dir_iter = std::filesystem::directory_iterator(_backup_dir);
        auto oldest_entry = std::make_optional<std::filesystem::directory_entry>();
        std::tm oldest_entry_date;

        for (auto entry : dir_iter) {

            if (entry.is_regular_file()) {

                // Get time from filename
                const auto new_entry_name = entry.path().stem();

                std::tm new_entry_date;
                strptime(new_entry_name.c_str(), "%Y-%m-%d.%X", &new_entry_date);

                if (!oldest_entry.has_value()) {
                    oldest_entry = entry;
                    oldest_entry_date = new_entry_date;
                    continue;
                }

                if (difftime(mktime(&new_entry_date), mktime(&oldest_entry_date)) < 0) {
                    oldest_entry = entry;
                    oldest_entry_date = new_entry_date;
                }

            }
        }

        if (oldest_entry.has_value()) {
            if (std::filesystem::remove(oldest_entry->path())) {
                spdlog::info("Removed file at {0}", oldest_entry->path().c_str());

            } else {
                spdlog::error("Could not remove file at {0}", oldest_entry->path().c_str());
            }

        } else {
            spdlog::error("Could not remove entry, no file found");
        }
    }

    spdlog::info("Backup Complete!");
}

// TODO: disarm backup timer