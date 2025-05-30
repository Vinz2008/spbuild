#include <string>
#include <vector>

std::string read_file(std::string_view filename);
bool file_exists(std::string_view filename);

std::vector<std::string> wildcard_files(std::string folder, std::string extension);
std::string_view get_extension(std::string_view filename);

bool exe_is_in_path(std::string program);
std::string strip_file_extension(std::string_view filename);

std::string get_tmp_directory();
std::string append_path(std::string path, std::string add_path);

bool delete_file(std::string_view path);