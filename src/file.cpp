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


std::string_view get_extension(std::string_view filename){
    if (filename[0] == '.'){
        return "";
    }

    int dot_pos = filename.size() - 1;
    while (filename[dot_pos] != '.'){
        dot_pos--;
    }
    return filename.substr(dot_pos, filename.size()-1);
}

// return filenames with path
// ex : wildcard_files("src/", ".c") -> ["src/main.c", "src/test.c"]
std::vector<std::string> wildcard_files(std::string folder, std::string extension){
    std::vector<std::string> files;

    for (const auto& dir_entry : fs::directory_iterator(folder)) {
        if (dir_entry.is_regular_file()){
            const auto file_path = dir_entry.path().string();
            if (get_extension(file_path) == extension){
                files.push_back(file_path);
            }
        }
        

    }

    return files;
}