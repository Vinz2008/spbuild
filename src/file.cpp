#include "file.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string read_file(std::string_view filename){
    std::ifstream f(filename.cbegin(), std::ios::in);

    const uintmax_t file_size = fs::file_size(filename);
    std::string file_content = std::string(file_size, '\0');
    f.read(file_content.data(), file_size);

    return file_content;
}

bool file_exists(std::string_view filename){
    return fs::exists(filename);
}