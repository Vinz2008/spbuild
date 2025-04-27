#ifndef _BUILD_HEADER_
#define _BUILD_HEADER_

class Expr;

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>


class Executable {
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

#endif