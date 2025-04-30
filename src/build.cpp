#include "build.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "utils.h"
#include "file.h"
#include "lang.h"

void interpret_toplevel_function_call(Build& build, std::string_view function_name, std::vector<std::unique_ptr<Expr>> args){
    if (function_name == "exe"){
        std::unique_ptr<String> out_filename = downcast_expr<String>(std::move(args[0]));
        std::unique_ptr<Array> sources = downcast_expr<Array>(std::move(args[1]));
        build.executables.push_back(Executable(out_filename->s, sources->arr));
    } else {
        fprintf(stderr, "ERROR : unknown toplevel function name %s\n", function_name.data());
    }
}

struct WildcardFormat {
    std::string folder;
    std::string extension;
    WildcardFormat(std::string folder, std::string extension) : folder(folder), extension(extension) {}
};

static WildcardFormat parse_wildcard_format(std::string format){
    int slash_pos = format.size() - 1;
    while (format[slash_pos] != '/'){
        slash_pos--;
    }
    std::string folder = format.substr(0, slash_pos);
    
    if (format[slash_pos+1] != '*'){
        fprintf(stderr, "ERROR : missing '*' in wildcard format");
        exit(1);
    }

    int dot_pos = slash_pos+2;
    if (format[dot_pos] != '.'){
        fprintf(stderr, "ERROR : missing '.' in wildcard format extension");
        exit(1);
    }

    std::string extension = format.substr(dot_pos, format.size()-1);

    return WildcardFormat(folder, extension);
}

std::unique_ptr<Expr> interpret_expr_function_call(std::string_view function_name, std::vector<std::unique_ptr<Expr>> args){
    if (function_name == "wildcard"){
        // format in pattern 'folder/*.ext', ex : 'src/*.c'
        std::unique_ptr<String> format_expr = downcast_expr<String>(std::move(args[0]));
        WildcardFormat wildcard_format = parse_wildcard_format(format_expr->s);
        std::vector<std::string> files = wildcard_files(wildcard_format.folder, wildcard_format.extension);
        return std::make_unique<Array>(files);
    } else {
        fprintf(stderr, "ERROR : unknown function name %s\n", function_name.data());
        exit(1);
    }
}


BackendType parse_backend_type(std::string backend_str){
    if (backend_str == "Ninja"){
        return NINJA;
    } else if (backend_str == "Makefile"){
        return MAKEFILE;
    } else {
        fprintf(stderr, "ERROR : unexpected backend\n");
        exit(1);
    }
}


// TODO : merge these code
// ex : instead of the two lines with the cc line, add a function called set_c_compiler which will write to the build file in function of the build type and call it : set_c_compiler(get_c_compiler())

static std::vector<std::string> get_obj_files(std::vector<std::string> sources){
    std::vector<std::string> objs;
    for (uint i = 0; i < sources.size(); i++){
        objs.push_back(sources[i] + ".o");
    }
    return objs;
}

// TODO : support for cpp, rust, etc

static std::string get_c_compiler_var(BackendType backend_type){
    switch (backend_type){
        case NINJA:
            return "$cc";
        case MAKEFILE:
            return "$(CC)";
        default:
            fprintf(stderr, "ERROR : unknown backend\n");
            exit(1);
    }
}


static std::string get_linker_var(BackendType backend_type){
    return get_c_compiler_var(backend_type);
}

static void gen_makefile_clean(std::stringstream& stream, std::vector<std::string>& files){
    stream << "clean:\n";
    stream << "\trm -f ";
    for (uint i = 0; i < files.size(); i++){
        stream << files[i] << " ";
    }
    stream << "\n\n";
}

static void add_generic_linker_rule(BackendType backend_type, std::stringstream& stream){
    assert(backend_type != MAKEFILE);
    stream << "rule ld\n";
    stream << "  command = " << get_linker_var(backend_type) << " -o $out $in $ldflags\n\n";
    
}

static void gen_exe_target(BackendType backend_type, std::stringstream& stream, std::string output_file, std::vector<std::string>& objs){
    switch (backend_type){
        case NINJA:
            stream << "build " << output_file << ": ld ";
            break;
        case MAKEFILE:
            stream << output_file << ": ";
            break;
    }
    for (uint j = 0; j < objs.size(); j++){
        stream << objs[j] << " ";
    }
    stream << "\n";

    if (backend_type == MAKEFILE){
        stream << "\t" << get_linker_var(backend_type) << " $(LDFLAGS) -o " << output_file << " ";
        for (uint j = 0; j < objs.size(); j++){
            stream << objs[j] << " ";
        }
        stream << " \n\n";
    }
    
}

static void gen_source_target(BackendType backend_type, std::stringstream& stream, std::string source_file, std::string object_file){
    // TODO : add function to lang.cpp to get compiler for language
    switch (backend_type){
        case NINJA:
            stream << "build " << object_file << ": cc " << source_file << "\n\n";
            break;
        case MAKEFILE:
            stream << object_file << ":\n";
            stream <<  "\t$(CC) $(CFLAGS) -c -o " << object_file << " " << source_file <<  " \n\n";
            break;
    }
    
}

static std::string gen_build_backend(Build build, BackendType backend_type){
    std::stringstream stream;
    add_language_support(backend_type, C, stream);
    if (backend_type != MAKEFILE){
        add_generic_linker_rule(backend_type, stream);
    }

    std::vector<std::string> all_objs;

    for (uint i = 0; i < build.executables.size(); i++){
        
        
        std::vector<std::string> objs = get_obj_files(build.executables[i].sources);
        

        gen_exe_target(backend_type, stream, build.executables[i].output_file, objs);
        

        for (uint j = 0; j < build.executables[i].sources.size(); j++){
            std::string source_file = build.executables[i].sources[j];
            std::string object_file = objs[j];
            gen_source_target(backend_type, stream, source_file, object_file);
        }
        
        all_objs.push_back(build.executables[i].output_file);
        all_objs.insert(all_objs.end(), objs.begin(), objs.end());
    }

    if (backend_type == MAKEFILE){
        gen_makefile_clean(stream, all_objs);
    }
    
    
    return stream.str();
}

static std::string get_out_filename(BackendType backend_type){
    switch (backend_type){
        case NINJA:
            return "build.ninja";
        case MAKEFILE:
            return "Makefile";
        default:
            fprintf(stderr, "unexpected build system : %d\n", backend_type);
            exit(1);
    }
}

void gen_build(Build build, BackendType backend_type){
    std::string file_content = gen_build_backend(std::move(build), backend_type);

    std::string out_filename = get_out_filename(backend_type);
    std::ofstream f(out_filename);
    f.write(file_content.c_str(), file_content.size());
    f.close();
}