#ifndef _BUILD_HEADER_
#define _BUILD_HEADER_

#include "parser.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>


class Executable {
public:
    std::string output_file;
    std::vector<std::string> sources;
    Executable(std::string output_file, std::vector<std::string> sources) : output_file(output_file), sources(sources) {}
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
    std::vector<Executable> executables;
    Build(){}
};

void interpret_function_call(Build& build, std::string_view function_name, std::vector<std::unique_ptr<Expr>> args);

enum BackendType {
    NINJA,
    MAKEFILE,
};

void gen_build(Build build, enum BackendType backend_type);

#endif