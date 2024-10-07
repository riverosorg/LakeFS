// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <sys/socket.h>
#include <sys/un.h>

#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <spdlog/spdlog.h>

#include "command_interface.h"
#include "control.hpp"
#include "db.hpp"
#include "sqlite/sqlite3.h"

std::string mount_point = "/lakefs";

// Runs the socket server to control the FS
void control_server() {
    // Create the socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) { 
        spdlog::critical("Failed to create socket");

        exit(1);
    }

    // Bind the socket
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, LAKE_SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        spdlog::critical("Failed to bind socket");

        exit(1);
    }

    // Listen on the socket
    if (listen(server_fd, 5) == -1) {
        spdlog::critical("Failed to listen on socket");

        exit(1);
    }

    // Accept connections
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            spdlog::critical("Failed to accept connection");

            exit(1);
        }

        // Read the command
        char buffer[1024];
        int bytes_read = read(client_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            spdlog::critical("Failed to read from socket");

            exit(1);
        }

        // Parse the command
        lake_command_t* command = (lake_command_t*)buffer;
        switch (command->command) {
            case LAKE_ADD_FILE: {
                std::string path = std::string(command->data, command->size);
                int rc = db_add_file(path);

                if (rc != SQLITE_OK) {
                    spdlog::error("Failed to add file to database");
                } else {
                    spdlog::info("Added file to database: {0}", path);
                }

                break;
            }
            case LAKE_TAG_FILE: {
                 std::string cmd_data = std::string(command->data, command->size);

                const std::string delimiter = "\n";
                size_t pos = cmd_data.find(delimiter);
                std::string path = cmd_data.substr(0, pos);
                std::string tag = cmd_data.substr(pos + delimiter.length());
                 
                int rc = db_tag_file(path, tag);

                if (rc != SQLITE_OK) {
                    spdlog::error("Failed to tag file in database");
                } else {
                    spdlog::info("Tagged file in database: {0} with tag: {1}", path, tag);
                }
             
                break;
            }
            case LAKE_SET_DEFAULT_QUERY: {
                std::string query = std::string(command->data, command->size);
                spdlog::info("Setting default query to: {0}", query);

                db_set_default_query(query);

                break;
            }
        };
    }
}