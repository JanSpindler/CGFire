//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_UTIL_HPP
#define CGFIRE_UTIL_HPP

#include <vector>

namespace en
{
    unsigned int PopGLError(bool exit);

    std::vector<char> ReadFileBinary(const char* filename);

    class Log
    {
    public:
        static void Info(const char* msg);
        static void Warn(const char* msg);
        static void Error(const char* msg, bool exit);
    };
}

#endif //CGFIRE_UTIL_HPP
