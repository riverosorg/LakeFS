// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#include <algorithm>
#include <spdlog/spdlog.h>
#include <vector>
#include <filesystem>

#include "query_lang/parser.hpp"
#include "db.hpp"
#include "utilities.hpp"

std::expected<std::vector<std::string>, int> get_files(const char* path) {
    spdlog::trace("Entering get_files(path={0})", path);

    std::vector<std::string> files;

    // Get path segments from query
    const auto path_segments = split(path, "/");

    const bool path_has_query = path_segments.size() != 0 && path_segments[0].contains('(');

    if (path_has_query)
    {
        const auto query = extract_query(path);
        const auto query_ast = parse(query);

        if (query_ast.has_value()) {
            files = db_run_query(query_ast.value());
        
        } else {
            spdlog::error("Error while parsing, invalid query");

            return std::unexpected(-EINVAL);
        }
    }
    else
    {
        files = db_run_default_query();
    }

    if (path_segments.size() > 1)
    {

        // Get the file path by comparing the file name to the query results
        std::string file_path;
        std::string looking_for_file;

        if (path_has_query)
        {
            looking_for_file = path_segments[1];
        }
        else
        {
            looking_for_file = path_segments[0];
        }


        for (const auto& query_file : files) {
            
            const std::string query_file_name = 
                query_file.substr(query_file.find_last_of("/") + 1);

            if (query_file_name == looking_for_file) {
                file_path = query_file;
                break;
            }
        }

        // Have to drill down arbitry path lengths
        // Take current real path, get dir entries
        // look for next folder, get that real path
        // repeat
        for (int i = 1 + path_has_query; i < path_segments.size(); i++)
        {
            auto finding_dir = path_segments[i];

            const auto dir_iter = std::filesystem::directory_iterator(file_path);

            for (const auto& dir_entry : dir_iter)
            {
                if (dir_entry.path().filename() == finding_dir)
                {
                    file_path = dir_entry.path();
                    break;
                }
            }
        }

        if (std::filesystem::is_directory(file_path))
        {
            // Query is for a folder
            spdlog::debug("Found real dir path: {0}", file_path.c_str());

            // Get files of our actual destination folder
            files.clear();

            for (auto const& dir_entry : std::filesystem::directory_iterator{file_path})
            {
                files.push_back(dir_entry.path());
            }
        }
    }

    spdlog::trace("Exiting get_files() -> vec.size()={0}", files.size());
    return files;
}

std::expected<std::string, int> reverse_query(const char* path) {
    auto path_s = std::string(path);

    spdlog::trace("Entering reverse_query(path={0})", path_s);

    const auto path_segments = split(path, "/");

    std::string looking_for_file = path_segments[path_segments.size()-1];

    std::string reconstructed_path;

    for (int i = 0; i < path_segments.size()-1; i++)
    {
        reconstructed_path += "/" + path_segments[i];
    }

    const auto files = get_files(reconstructed_path.c_str());

    if (!files.has_value())
    {
        return std::unexpected(files.error());
    }

    const auto file_path = std::find_if(files->begin(), files->end(), 
        [looking_for_file] (const std::string& file_name) -> bool {
            return std::filesystem::path(file_name).filename() == looking_for_file;
    });

    spdlog::trace("Exiting reverse_query() -> {0}", *file_path);
    return *file_path;
}

std::string extract_query(const char* path) {
    spdlog::trace("Entering extract_query(path={0})", std::string(path));

    auto path_s = std::string(path);

    // TODO: This is pretty niave. quickly 'fixed' for now but a real algorithm should be used
    const auto extracted_query = 
        path_s.substr(path_s.find_first_of('('), path_s.find_last_of(')') - path_s.find_first_of('(') + 1);

    spdlog::trace("Exiting extract_query() -> {0}", extracted_query);
    return extracted_query;
}

std::vector<std::string> split(const std::string str, const std::string delim) {
    spdlog::trace("Entering split(str={0}, delim={1})", str, delim);

    std::vector<std::string> parts;

    int part_start = 0;
    for (int i = 0; i < str.size(); i++)
    {
        const std::string delim_match = str.substr(i, delim.size());

        if (delim_match == delim)
        {
            if (part_start != i)
            {
                parts.push_back(str.substr(part_start, i-part_start));
            }

            part_start = i + delim.size();
        }
    }

    if (part_start < str.size())
    {
        parts.push_back(str.substr(part_start, str.size() - part_start));
    }

    spdlog::trace("Exiting split() -> vec.size()={0}", parts.size());
    return parts;
}