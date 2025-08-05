// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "backups.hpp"
#include "db.hpp"

#include <filesystem>
#include <optional>

#include <pthread.h>
#include <sys/signal.h>
#include <time.h>

#include <spdlog/spdlog.h>

// better way to pass these in? handler args
uint32_t _number_backups;
std::string _backup_dir;

static auto handle_backup(sigval val) -> void;

auto create_backup_timer(std::chrono::seconds interval, uint32_t number_backups, std::string backup_path) -> void {
    spdlog::info("Setting up backups to keep {0} copies and run every {1} hours", 
        number_backups, 
        std::chrono::duration_cast<std::chrono::hours>(interval).count());

    _number_backups = number_backups;
    _backup_dir = backup_path;

    // Create a C timer    
    sigevent event;

    pthread_attr_t backup_thread_attr;
    pthread_attr_init(&backup_thread_attr);

    // Will spawn signal handler in its own thread
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = &handle_backup;
    event.sigev_notify_attributes = &backup_thread_attr;

    timer_t timer_id;
    if (timer_create(CLOCK_MONOTONIC, &event, &timer_id) == -1) {
        spdlog::critical("Could not create timer! {0}", strerror(errno));
        // todo: pass up
    }

    itimerspec timer_spec = {};
    timer_spec.it_interval.tv_sec = interval.count();
    timer_spec.it_value.tv_sec = 30;

    spdlog::debug("Making timer for {0} seconds", timer_spec.it_interval.tv_sec);

    //start timer
    if (timer_settime(timer_id, 0, &timer_spec, nullptr) == -1) {
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

                // TODO: if this is removed, the backup fails!
                spdlog::debug("Looking at file {0} {1}", entry.path().c_str(), entry.path().stem().c_str());

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