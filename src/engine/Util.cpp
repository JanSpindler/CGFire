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

    void ClearGLError()
    {
        while (glGetError() != GL_NO_ERROR);
    }

    std::vector<char> ReadFileBinary(const std::string& filename)
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

    void Log::Info(const std::string& msg)
    {
        //std::cout << "INFO:  " << msg << std::endl;
    }

    void Log::Warn(const std::string& msg)
    {
        std::cout << "Warn:  " << msg << std::endl;
    }

    void Log::Error(const std::string& msg, bool exit)
    {
        std::cout << "Error: " << msg << std::endl;
        if (exit)
            throw std::runtime_error("Closing Application due to Error");
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> Time::last_ = std::chrono::high_resolution_clock::now();
    double Time::deltaTime_ = 0.0f;
    int Time::fps_ = 0;

    void Time::Update()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::nanoseconds delta = now - last_;
        last_ = now;
        deltaTime_ = (double) delta.count() / 1000000000.0;

        // TODO: implement fps
    }

    double Time::GetDeltaTime()
    {
        return deltaTime_;
    }

    int Time::GetFps()
    {
        return fps_;
    }
}
