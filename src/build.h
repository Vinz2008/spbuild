#ifndef _BUILD_HEADER_
#define _BUILD_HEADER_

#include "parser.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "lang.h"
#include "thread_pool.h"

// TODO : parallelize generating exes (make it a thread pool task)
class Executable {
public:
    std::string output_file;
    std::vector<std::string> sources;
    std::vector<std::string> libraries;
    Executable(std::string output_file, std::vector<std::string> sources, std::vector<std::string> libraries) : output_file(output_file), sources(sources), libraries(libraries) {}
};

class Var {
public:
    std::string name;
    std::unique_ptr<Expr> val;
    Var(std::string name, std::unique_ptr<Expr> val) : name(name), val(std::move(val)) {}
};

class Build {
public:
    std::unordered_map<std::string, std::unique_ptr<Var>> vars;
    std::unordered_map<Language, std::string, EnumClassHash> compiler_paths;
    std::vector<Executable> executables;
    std::vector<std::unique_ptr<ParallelTask>> parallel_tasks;
    Build(){}
};

void interpret_toplevel_function_call(Build& build, std::string_view function_name, std::vector<std::unique_ptr<Expr>> args);

std::unique_ptr<Expr> interpret_expr_function_call(std::string_view function_name, std::vector<std::unique_ptr<Expr>> args);

BackendType parse_backend_type(std::string backend_str);

void gen_build(Build build, BackendType backend_type);

#endif