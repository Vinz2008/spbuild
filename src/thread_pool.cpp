#include "thread_pool.h"
#include <thread>
#include <fstream>
#include <stdarg.h>
#include "build.h"
#include "utils.h"
#include "file.h"

std::mutex print_mutex;

void multithreaded_print(const std::string& s){
    std::unique_lock<std::mutex> lock(print_mutex);
    /*printf("%s", s.c_str());
    fflush(stdout);*/
    std::cout << "-- " << s << std::flush;
}

uint32_t get_thread_nb(){
    return std::thread::hardware_concurrency();
}

TaskOutput HeaderCheck::run(Build& build) {
    multithreaded_print("checking " + header_name + "        \n");
    std::string c_compiler = "cc";
    if (build.compiler_paths.find(C) != build.compiler_paths.end()){
        c_compiler = build.compiler_paths[C];
    }

    std::string tmp_dir = get_tmp_directory();
    std::string src_test_filename = std::string(strip_file_extension(header_name)) + ".cpp";
    std::string src_test_path = append_path(tmp_dir, src_test_filename);


    std::ofstream src_test_f(src_test_path);

    src_test_f << "#include <" << header_name << ">\n";


    src_test_f.close();

    
    std::pair<int, std::optional<std::string>> out_cc = run_cmd(c_compiler + " -E " + src_test_path);

    delete_file(src_test_path);
    
    if (out_cc.first != 0){
        // failure
        return TaskOutput(false, "header " + header_name + " not found");
    }

    return TaskOutput(true);
}


static void run_thread(ParallelUniqueQueue<ParallelTask>& queue, Build& build){
    while (!queue.empty()){
        std::unique_ptr<ParallelTask> task = queue.pop();
        if (!task){
            return;
        }
        task->run(build);
    }
}

void launch_thread_pool(Build& build, uint32_t thread_nb){
    std::vector<std::thread> threads;
    thread_nb = std::min(build.parallel_tasks.size(), (size_t)thread_nb);

    ParallelUniqueQueue<ParallelTask> queue(std::move(build.parallel_tasks));
    threads.reserve(thread_nb);
    for (uint32_t i = 0; i < thread_nb; i++){
        threads.push_back(std::thread(run_thread, std::ref(queue),  std::ref(build)));
    }


    for (uint32_t i = 0; i < threads.size(); i++){
        threads[i].join();
    }
}