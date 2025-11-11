// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cstdio>
#include <cstring>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <string>
#include <unistd.h>

#include "command_interface.h"
#include "control.hpp"
#include "db.hpp"

// Runs the socket server to control the FS
void control_server()
{
    // Create the socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        spdlog::critical("Failed to create socket");

        exit(1);
    }

    // Bind the socket
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, LAKE_SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        spdlog::critical("Failed to bind socket: {0}", strerror(errno));

        exit(1);
    }

    // Listen on the socket
    if (listen(server_fd, 5) == -1)
    {
        spdlog::critical("Failed to listen on socket");

        exit(1);
    }

    // Accept connections
    while (true)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1)
        {
            spdlog::critical("Failed to accept connection");

            exit(1);
        }

        while (true)
        {

            // Read the command
            char buffer[2048]; // TODO: Variable size!!
            int bytes_read = read(client_fd, buffer, sizeof(buffer));
            if (bytes_read == -1)
            {
                spdlog::critical("Failed to read from socket");

                exit(1);
            }

            if (bytes_read == 0)
            {
                // Socket disconnected
                spdlog::trace("Socket connection closed");
                close(client_fd);
                break;
            }

            // Parse the command
            lake_command_t* command = (lake_command_t*)buffer;
            switch (command->command)
            {
            case LAKE_TAG_FILE:
            {
                std::string cmd_data = std::string(command->data, command->size);

                const std::string delimiter = "\n";
                size_t pos = cmd_data.find(delimiter);
                std::string path = cmd_data.substr(0, pos);
                std::string tag = cmd_data.substr(pos + delimiter.length());

                spdlog::info("Tagging file in database: {0} with tag: {1}", path, tag);

                int rc = db_tag_file(path, tag);

                if (rc != SQLITE_OK)
                {
                    spdlog::error("Failed to tag file in database: {0}", rc);
                }

                break;
            }
            case LAKE_REMOVE_FILE:
            {
                std::string path = std::string(command->data, command->size);
                spdlog::info("Removing file from database: {0}", path);

                int rc = db_remove_file(path);

                if (rc != SQLITE_OK)
                {
                    spdlog::error("Failed to remove file from database: {0}", rc);
                }

                break;
            }
            case LAKE_REMOVE_TAG:
            {
                std::string cmd_data = std::string(command->data, command->size);

                const std::string delimiter = "\n";
                size_t pos = cmd_data.find(delimiter);
                std::string path = cmd_data.substr(0, pos);
                std::string tag = cmd_data.substr(pos + delimiter.length());

                spdlog::info("Removing tag from file in database: {0} with tag: {1}", path, tag);

                int rc = db_remove_tag(path, tag);

                if (rc != SQLITE_OK)
                {
                    spdlog::error("Failed to remove tag from file in database: {0}", rc);
                }

                break;
            }
            case LAKE_RELINK_FILE:
            {
                std::string cmd_data = std::string(command->data, command->size);

                const std::string delimiter = "\n";
                size_t pos = cmd_data.find(delimiter);
                std::string old_path = cmd_data.substr(0, pos);
                std::string new_path = cmd_data.substr(pos + delimiter.length());

                int rc = db_relink_file(old_path, new_path);

                if (rc != SQLITE_OK)
                {
                    spdlog::error("Failed to relink file: {0}", rc);
                }

                break;
            }
            case LAKE_SET_DEFAULT_QUERY:
            {
                std::string query = std::string(command->data, command->size);
                spdlog::info("Setting default query to: {0}", query);

                db_set_default_query(query);

                break;
            }

            default:
            {
                spdlog::error("Unrecognized message received");

                break;
            }
            };
        }
    }
}