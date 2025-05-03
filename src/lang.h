#ifndef _LANG_HEADER_
#define _LANG_HEADER_

#include "backend.h"
#include <sstream>

class Build;

enum Language {
    C,
};

void add_language_support(Build& build, BackendType backend_type, Language lang, std::stringstream& stream);

#endif