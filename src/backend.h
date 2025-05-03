#ifndef _BACKEND_HEADER_
#define _BACKEND_HEADER_

#include <cstdint>

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

enum BackendType {
    NINJA,
    MAKEFILE,
};

#endif