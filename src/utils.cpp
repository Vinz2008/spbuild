#include "utils.h"
#include <array>
#include <optional>

std::pair<int, std::optional<std::string>> run_cmd(const std::string& cmd, bool keep_output){
    int exit_status = 0;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr){
        fprintf(stderr, "ERROR : cannot open pipe\n");
        exit(1);
    }
    std::optional<std::string> result = std::nullopt;
    if (keep_output){
        std::array<char, 256> buffer;
        result = "";
        while(not std::feof(pipe)){
            size_t bytes = std::fread(buffer.data(), 1, buffer.size(), pipe);
            result->append(buffer.data(), bytes);
        }
    }
    int rc = pclose(pipe);
    if (WEXITSTATUS(rc)){
        exit_status = WEXITSTATUS(rc);
    }

    return std::make_pair(exit_status, result);
}