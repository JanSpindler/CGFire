//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include "engine/Util.hpp"

namespace en
{
    unsigned int PopGLError(bool exit)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::string errorStr = "OpenGL Error " + std::to_string(error);
            Log::Error(errorStr.c_str(), exit);
        }
        return error;
    }

    std::vector<char> ReadFileBinary(const char* filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file)
        {
            Log::Error("Failed to open file", true);
            return std::vector<char>(0);
        }

        size_t fileSize = (size_t) file.tellg();
        file.seekg(0);

        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void Log::Info(const char* msg)
    {
        std::cout << "INFO:  " << msg << std::endl;
    }

    void Log::Warn(const char* msg)
    {
        std::cout << "Warn:  " << msg << std::endl;
    }

    void Log::Error(const char* msg, bool exit)
    {
        std::cout << "Error: " << msg << std::endl;
        if (exit)
            throw std::runtime_error("Closing Application due to Error");
    }
}
