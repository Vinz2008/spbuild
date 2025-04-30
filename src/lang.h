#include "build.h"

enum Language {
    C,
};

void add_language_support(BackendType backend_type, Language lang, std::stringstream& stream);