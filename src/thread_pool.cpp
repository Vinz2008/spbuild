#include "thread_pool.h"
#include "build.h"
#include "file.h"
#include "utils.h"

bool HeaderCheck::run(Build& build) {
    std::string c_compiler = "";
    if (build.compiler_paths.find(C) != build.compiler_paths.end()){
        c_compiler = build.compiler_paths[C];
    }

    std::string tmp_dir = get_tmp_directory();
    std::string src_test_filename = std::string(strip_file_extension(header_name)) + ".cpp";
    std::string src_test_path = append_path(tmp_dir, src_test_filename);

    return true;
}


void launch_thread_pool(Build& build){
    uint thread_nb = get_thread_nb();
    uint nb_tasks_done = 0;
    //while (nb_tasks_done < build.parallel_tasks.size()){}
}