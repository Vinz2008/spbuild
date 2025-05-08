#ifndef _PARALLEL_POOL_
#define _PARALLEL_POOL_

#include <string>

class Build;

class ParallelTask {
public:
    virtual bool run(Build& build) = 0;
    virtual ~ParallelTask() = default;
};

class HeaderCheck : public ParallelTask {
public:
    std::string header_name;
    std::string header_define;
    HeaderCheck(std::string header_name, std::string header_define) : header_name(header_name), header_define(header_define) {}
    bool run(Build& build) override;
};

void launch_thread_pool(Build& build);

#endif