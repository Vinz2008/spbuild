#include <stdlib.h>
#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "file.h"
#include "build.h"

int main(int argc, char** argv){
    std::string backend = "";
    std::string filename = "build.sp";
    for (int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if (arg == "-G"){
            i++;
            backend = argv[i];
        } else {
            filename = arg;
        }
    }


    BackendType backend_type = NINJA;
    if (backend != ""){
        backend_type = parse_backend_type(backend);
    }

    if (!file_exists(filename)){
        fprintf(stderr, "ERROR : File %s not found\n", filename.c_str());
        exit(1);
    }

    std::string file_content = read_file(filename);
    std::cout << file_content << std::endl;
    std::vector<Token> tokens = lex(file_content);
    Build build = parse(tokens);

    gen_build(std::move(build), backend_type);
    destroy_tokens(tokens);
    return 0;
}