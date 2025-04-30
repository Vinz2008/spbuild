#include "lang.h"
#include <sstream>
#include "build.h"

static std::string get_c_compiler_path(){
    // return TODO
    return "cc";
}



static void set_cc_compiler(BackendType backend_type, std::stringstream& stream, std::string path){
    switch (backend_type){
        case NINJA:
            stream << "cc = " << path << "\n\n";
            break;
        case MAKEFILE:
            stream << "CC = " << path << "\n\n";
            break;

    }
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

void add_language_support(BackendType backend_type, Language lang, std::stringstream& stream){
    switch (lang){
        case C:
            set_cc_compiler(backend_type, stream, get_c_compiler_path());
            add_rules_for_c(backend_type, stream);
            break;
    
    }
    
}