#include <stdlib.h>
#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "file.h"
#include "build.h"

int main(int argc, char** argv){
    std::string filename = "build.sp";
    for (int i = 1; i < argc; i++){
        filename = argv[i];
    }

    if (!file_exists(filename)){
        fprintf(stderr, "ERROR : File %s not found\n", filename.c_str());
        exit(1);
    }

    std::string file_content = read_file(filename);
    std::cout << file_content << std::endl;
    std::vector<Token> tokens = lex(file_content);
    Build build = parse(tokens);

    gen_build(std::move(build), MAKEFILE);
    destroy_tokens(tokens);
    return 0;
}