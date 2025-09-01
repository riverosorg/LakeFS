#include <spdlog/spdlog.h>
#include <vector>

#include "query_lang/parser.hpp"
#include "db.hpp"
#include "utilities.hpp"

std::string reverse_query(const char* path) {
    auto path_s = std::string(path);

    spdlog::trace("Entering reverse_query(path={0})", path_s);

    std::vector<std::string> query_files;

    if (path_s.find_first_of('(') == std::string::npos) {
        query_files = db_run_default_query();

    } else {
        std::string query = extract_query(path);

        try {
            const auto query_ast = parse(query);

            if (query_ast.has_value()) {
                query_files = db_run_query(query_ast.value());
            
            } else {
                spdlog::error("Error while parsing, no value returned");

                return "";
            }
        
        } catch (std::exception err) {
            spdlog::error("Error while parsing: {0}", err.what());
            
            return "";
        }
    }

    // Get the file path by comparing the file name to the query results
    std::string file_path;

    const std::string looking_for_file = 
        std::string(path_s).substr(std::string(path_s).find_last_of("/") + 1);

    for (const auto& query_file : query_files) {
        
        const std::string query_file_name = 
            query_file.substr(query_file.find_last_of("/") + 1);

        if (query_file_name == looking_for_file) {
            file_path = query_file;
            break;
        }
    }

    spdlog::trace("Exiting reverse_query() -> {0}", file_path);
    return file_path;
}

std::string extract_query(const char* path) {
    spdlog::trace("Entering extract_query(path={0})", std::string(path));

    auto path_s = std::string(path);

    // TODO: This is pretty niave. quickly 'fixed' for now but a real algorithm should be used
    const auto extracted_query = 
        path_s.substr(path_s.find_first_of('('), path_s.find_last_of(')') - path_s.find_first_of('(') + 1);

    spdlog::trace("Exiting parse() -> {0}", extracted_query);
    return extracted_query;
}