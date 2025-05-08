#include "build.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <set>
#include "utils.h"
#include "file.h"
#include "lang.h"

static const std::string_view ldflags_global = std::string_view("ldflags_global");

// deduplicated targets, there could be a problem with different cflags/configs (TODO)
std::set<std::string> targets_added;

void interpret_toplevel_function_call(Build& build, std::string_view function_name, std::vector<std::unique_ptr<Expr>> args){
    if (function_name == "exe"){
        std::unique_ptr<String> out_filename = downcast_expr<String>(std::move(args[0]));
        std::unique_ptr<Array> sources = downcast_expr<Array>(std::move(args[1]));
        std::vector<std::string> libraries;
        if (args.size() >= 3){
            std::unique_ptr<Array> libraries_expr = downcast_expr<Array>(std::move(args[2]));
            libraries = libraries_expr->arr;
        }
        build.executables.push_back(Executable(out_filename->s, sources->arr, libraries));
    } else if (function_name == "cc"){
        std::unique_ptr<String> cc_path = downcast_expr<String>(std::move(args[0]));
        build.compiler_paths[C] = cc_path->s; 
    } else if (function_name == "header_check"){
        std::unique_ptr<String> header_name = downcast_expr<String>(std::move(args[0]));
        std::string header_define = ""; 
        if (args.size() >= 1){
            std::unique_ptr<String> header_define_expr = downcast_expr<String>(std::move(args[0]));
            header_define = header_define_expr->s;
        }
        build.parallel_tasks.push_back(std::make_unique<HeaderCheck>(header_name->s, header_define));
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
    for (uint32_t i = 0; i < sources.size(); i++){
        objs.push_back(sources[i] + ".o");
    }
    return objs;
}


static void set_build_var(std::stringstream& stream, std::string_view var_name, std::string_view val){
    stream << var_name << " = " << val << "\n\n";
}

static std::string get_build_var(BackendType backend_type, std::string var_name){
    switch (backend_type){
        case NINJA:
            return "$" + var_name;
        case MAKEFILE:
            return "$(" + var_name + ")";
        default:
            fprintf(stderr, "ERROR : unknown backend\n");
            exit(1);
    }
}

// TODO : support for cpp, rust, etc

static std::string get_c_compiler_var(BackendType backend_type){
    return get_build_var(backend_type, "cc");
    /*switch (backend_type){
        case NINJA:
            return "$cc";
        case MAKEFILE:
            return "$(CC)";
        default:
            fprintf(stderr, "ERROR : unknown backend\n");
            exit(1);
    }*/
}


static std::string get_linker_var(BackendType backend_type){
    return get_c_compiler_var(backend_type);
}

static void gen_makefile_clean(std::stringstream& stream, std::vector<std::string>& files){
    stream << "clean:\n";
    stream << "\trm -f ";
    for (uint32_t i = 0; i < files.size(); i++){
        stream << files[i] << " ";
    }
    stream << "\n\n";
}

static void add_makefile_all_target(Build& build, std::stringstream& stream){
    stream << "all: ";
    for (uint32_t i = 0; i < build.executables.size(); i++){
        stream << build.executables[i].output_file << " ";
    }
    stream  << "\n\n";
}

static void add_generic_linker_rule(BackendType backend_type, std::stringstream& stream){
    assert(backend_type != MAKEFILE);
    stream << "rule ld\n";
    stream << "  command = " << get_linker_var(backend_type) << " -o $out $in $ldflags\n\n";   
}

/*static void set_ldflags_global_var(std::stringstream& stream, std::string ldflags){
    set_build_var(stream, "ldflags_global", ldflags);
    
    // switch (backend_type){
    //     case NINJA:
    //         stream << "ldflags = " << ldflags << "\n\n";
    //         break;
    //     case MAKEFILE:
    //         stream << "LDFLAGS = " << ldflags << "\n\n";
    //         break;
    // }
}*/

static void set_ldflags_global(/*BackendType backend_type,*/ std::stringstream& stream){
    std::string ldflags = " ";
    if (exe_is_in_path("mold")){
        ldflags += "-fuse-ld=mold ";
    }

    //set_ldflags_global_var(/*backend_type,*/ stream, ldflags);
    set_build_var(stream, "ldflags_global", ldflags);
}

static std::string get_exe_ldflags(std::vector<std::string>& libraries){
    // TODO
    return "";
}

static void gen_exe_target(BackendType backend_type, std::stringstream& stream, std::string output_file, std::vector<std::string>& objs, std::vector<std::string>& libraries){
    if (targets_added.find(output_file) != targets_added.end()){
        return;
    }
    
    // TODO : only add specific vars when needed
    std::string ldflags_specific_name = "ldflags_" + (std::string)strip_file_extension(output_file);
    std::string ldflags_specific_val = get_build_var(backend_type, "ldflags_global") + " " + get_exe_ldflags(libraries);
    set_build_var(stream, ldflags_specific_name, ldflags_specific_val);
    
    switch (backend_type){
        case NINJA:
            stream << "build " << output_file << ": ld ";
            break;
        case MAKEFILE:
            stream << output_file << ": ";
            break;
    }
    for (uint32_t j = 0; j < objs.size(); j++){
        stream << objs[j] << " ";
    }
    stream << "\n";

    switch (backend_type){
        case MAKEFILE:
            stream << "\t" << get_linker_var(backend_type) << " $(" << ldflags_specific_name << ") -o " << output_file << " ";
            for (uint32_t j = 0; j < objs.size(); j++){
                stream << objs[j] << " ";
            }
            stream << " \n\n";
            break;
        case NINJA:
            stream << "  ldflags = $" << ldflags_specific_name << "\n\n";
            break;
    }
    targets_added.insert(output_file);
    
}

static void gen_source_target(BackendType backend_type, std::stringstream& stream, std::string source_file, std::string object_file){
    if (targets_added.find(object_file) != targets_added.end()){
        return;
    }

    // TODO : add function to lang.cpp to get compiler for language
    switch (backend_type){
        case NINJA:
            stream << "build " << object_file << ": cc " << source_file << "\n\n";
            break;
        case MAKEFILE:
            stream << object_file << ":\n";
            stream <<  "\t$(cc) $(CFLAGS) -c -o " << object_file << " " << source_file <<  " \n\n";
            break;
    }

    targets_added.insert(object_file);
}

static std::string gen_build_backend(Build build, BackendType backend_type){
    std::stringstream stream;
    add_language_support(build, backend_type, C, stream);

    set_ldflags_global(/*backend_type,*/ stream);

    if (backend_type != MAKEFILE){
        add_generic_linker_rule(backend_type, stream);
    }


    if (backend_type == MAKEFILE){
        add_makefile_all_target(build, stream);
    }

    std::vector<std::string> all_objs;

    for (uint32_t i = 0; i < build.executables.size(); i++){
        
        
        std::vector<std::string> objs = get_obj_files(build.executables[i].sources);
        

        gen_exe_target(backend_type, stream, build.executables[i].output_file, objs, build.executables[i].libraries);
        

        for (uint32_t j = 0; j < build.executables[i].sources.size(); j++){
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
    launch_thread_pool(build);


    std::string file_content = gen_build_backend(std::move(build), backend_type);

    std::string out_filename = get_out_filename(backend_type);
    std::ofstream f(out_filename);
    f.write(file_content.c_str(), file_content.size());
    f.close();
}