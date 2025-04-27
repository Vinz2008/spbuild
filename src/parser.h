#ifndef _PARSER_HEADER_
#define _PARSER_HEADER_

#include "build.h"


#include <memory>


class ExprType {
public:
    ExprType() {}
};


class Expr {
public:
    ExprType type;
    Expr(ExprType type) : type(type) {}
    virtual ~Expr() = default;
};

// TODO : add more types of array

class Array : public Expr {
public:
    //std::vector<std::unique_ptr<Expr>> expr_arr;
    std::vector<std::string> arr;
    Array(std::vector<std::string> arr) : Expr(ExprType()),  arr(arr) {}
};

Build parse(std::vector<Token> tokens);

#endif