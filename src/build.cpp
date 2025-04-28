#include "build.h"
#include "utils.h"
#include <fstream>


void interpret_function_call(Build& build, std::string_view function_name, std::vector<std::unique_ptr<Expr>> args){
    if (function_name == "exe"){
        std::unique_ptr<String> out_filename = downcast_expr<String>(std::move(args[0]));
        std::unique_ptr<Array> sources = downcast_expr<Array>(std::move(args[1]));
        build.executables.push_back(Executable(out_filename->s, sources->arr));
    } else {
        fprintf(stderr, "ERROR : unknown function name %s\n", function_name.data());
    }
}


static void gen_build_ninja(Build build, std::ofstream& stream){
    fprintf(stderr, "TODO");
    exit(1);
}




static void gen_build_makefile(Build build, std::ofstream& stream){
    //stream << ".SUFFIXES: .c .S .o\n\n"; // TODO

    for (int i = 0; i < build.executables.size(); i++){
        stream << build.executables[i].output_file << ": ";
        for (int j = 0; j < build.executables[i].sources.size(); j++){
            stream << build.executables[i].sources[j] + ".o" << " ";
        }
        stream << "\n\n";

        for (int j = 0; j < build.executables[i].sources.size(); j++){
            std::string source_file = build.executables[i].sources[j];
            std::string object_file = source_file + ".o";
            stream << object_file << ":\n";
            stream <<  "\t$(CC) $(CFLAGS) -c -o " << object_file << " " << source_file <<  " \n\n";
        }
    }
}

static std::string get_out_filename(BackendType backend_type){
    switch (backend_type){
        case NINJA:
            return "build.ninja";
        case MAKEFILE:
            return "Makefile.sp";
        default:
            fprintf(stderr, "unexpected build system : %d\n", backend_type);
            exit(1);
    }
}

void gen_build(Build build, BackendType backend_type){
    std::string out_filename = get_out_filename(backend_type);
    std::ofstream f(out_filename);
    if (backend_type == NINJA){
        gen_build_ninja(std::move(build), f);
    } else if (backend_type == MAKEFILE){
        gen_build_makefile(std::move(build), f);
    } else {
        fprintf(stderr, "unexpected build system : %d\n", backend_type);
        exit(1);
    }

    f.close();
}