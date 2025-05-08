#include "file.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#if defined(_WIN32) || defined(WIN32) 
#include <windows.h>
#endif


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
            const std::string file_path = dir_entry.path().string();
            if (get_extension(file_path) == extension){
                files.push_back(file_path);
            }
        }
        

    }

    return files;
}

static std::vector<std::string> split(std::string_view s, char delim){
    std::vector<std::string> res;

    int next_delim_pos = s.find(delim);

    while (next_delim_pos != -1){
        res.push_back(std::string(s.substr(0, next_delim_pos)));
        s = s.substr(next_delim_pos+1, s.size()-1);
        next_delim_pos = s.find(delim);
    }

    return res;
}

bool exe_is_in_path(std::string program){
    std::string path_var = getenv("PATH");
    std::vector<std::string> paths = split(path_var, ':');

    for (uint i = 0; i < paths.size(); i++){
        for (const auto& dir_entry : fs::directory_iterator(paths[i])) {
            if (dir_entry.is_regular_file()){
                const std::string file_path = dir_entry.path().filename().string();
                if (file_path == program){
                    return true;
                }
            }
    
        }
    }

    return false;    
}



std::string_view strip_file_extension(std::string_view filename){
    fs::path p = filename;
    return p.replace_extension().string();
}

#ifdef __unix__
const char* temp_unix_vars[] = { "TMPDIR", "TMP", "TEMP" };
#endif


std::string get_tmp_directory(){
#ifdef __unix__
    for (int i = 0; i < sizeof(temp_unix_vars); i++){
        char* tmp_env = std::getenv(temp_unix_vars[i]);
        if (tmp_env){
            return std::string(tmp_env);
        }
    }


    return "/tmp";

#elif defined(_WIN32) || defined(WIN32)
    char buf[100];
    int res = GetTempPathA(100, buf);
    return std::string(buf);
#endif
}


std::string append_path(std::string path, std::string add_path){
    return fs::path(path) / add_path;
}

