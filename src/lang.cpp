#include "lang.h"
#include <sstream>
#include "build.h"
#include "file.h"

static std::string get_compiler_path(Build& build, Language lang){
    if (build.compiler_paths.find(lang) != build.compiler_paths.end()){
        return build.compiler_paths[lang];
    }
    // return TODO
    switch (lang){
        case C:
        default:
            return "cc";
    }
    
}



static void set_cc_compiler(BackendType backend_type, std::stringstream& stream, std::string path){
    stream << "cc = " << path << "\n\n";
    /*switch (backend_type){
        case NINJA:
            stream << "cc = " << path << "\n\n";
            break;
        case MAKEFILE:
            stream << "CC = " << path << "\n\n";
            break;
    }*/
}

static void add_rules_for_c(BackendType backend_type, std::stringstream& stream){
    switch (backend_type){
        case NINJA:
            stream << "rule cc\n";
            stream << "  command = $cc $cflags -c -o $out $in\n\n";
            break;
        case MAKEFILE:
            // do nothing
            break;
    }
}



void add_language_support(Build& build, BackendType backend_type, Language lang, std::stringstream& stream){
    std::string compiler_path = get_compiler_path(build, lang);
    switch (lang){
        case C:
            set_cc_compiler(backend_type, stream, compiler_path);
            add_rules_for_c(backend_type, stream);
            break;
    
    }
    
}