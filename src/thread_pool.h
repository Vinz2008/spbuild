#ifndef _PARALLEL_POOL_
#define _PARALLEL_POOL_

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <optional>
#include <cstdint>

#include "backend.h"

class Build;

// multi-consumer parallel queue that was already created before-hand

template <typename T>
class ParallelUniqueQueue {
    std::queue<std::unique_ptr<T>> queue;
    std::mutex mutex;
public:
    ParallelUniqueQueue(std::queue<std::unique_ptr<T>> q) : queue(std::move(q)) {} 

    bool empty(){
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    std::unique_ptr<T> pop(){
        std::unique_lock<std::mutex> lock(mutex);
        // TODO : if queue empty ?
        if (queue.empty()){
            return nullptr;
        }
        std::unique_ptr<T> e = std::move(queue.front());
        queue.pop();
        return e;
    }
};

template <typename T>
class ParallelVector {
    std::vector<T> vec;
    std::mutex mutex;
public:
    ParallelVector(){} 


    void push_back(T element){
        std::unique_lock<std::mutex> lock(mutex);
        vec.push_back(element);
        
    }

    // only in single-threaded mode !!
    std::vector<T> get_vec(){
        std::unique_lock<std::mutex> lock(mutex);
        return vec;
    }
};

class TaskOutput {
public:
    bool has_succeeded;
    std::string build_stream_str;
    std::vector<std::string> all_objs;
    std::optional<std::string> error_str;
    TaskOutput(bool has_succeeded, std::string build_stream_str = "", std::vector<std::string> all_objs = {}, std::optional<std::string> error_str = std::nullopt) : has_succeeded(has_succeeded), build_stream_str(build_stream_str), all_objs(all_objs), error_str(error_str) {}
    TaskOutput(bool has_succeeded, std::optional<std::string> error_str) : has_succeeded(has_succeeded), error_str(error_str) {}
};

class ParallelTask {
public:
    virtual TaskOutput run(Build& build, BackendType backend_type) = 0;
    virtual ~ParallelTask() = default;
};


class HeaderCheck : public ParallelTask {
public:
    std::string header_name;
    std::string header_define;
    HeaderCheck(std::string header_name, std::string header_define) : header_name(header_name), header_define(header_define) {}
    TaskOutput run(Build& build,BackendType backend_type) override;
};

class Executable : public ParallelTask {
public:
    std::string output_file;
    std::vector<std::string> sources;
    std::vector<std::string> libraries;
    Executable(std::string output_file, std::vector<std::string> sources, std::vector<std::string> libraries) : output_file(output_file), sources(sources), libraries(libraries) {}
    TaskOutput run(Build& build, BackendType backend_type) override;
};

void launch_thread_pool(std::stringstream& stream, std::vector<std::string>& all_objs, Build& build, BackendType backend_type, uint32_t thread_nb);

uint32_t get_thread_nb();

#endif