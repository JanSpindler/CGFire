//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_UTIL_HPP
#define CGFIRE_UTIL_HPP

#include <vector>
#include <string>
#include <chrono>

namespace en
{
    unsigned int PopGLError(bool exit);
    void ClearGLError();

    std::vector<char> ReadFileBinary(const std::string& filename);

    class Log
    {
    public:
        static void Info(const std::string& msg);
        static void Warn(const std::string& msg);
        static void Error(const std::string& msg, bool exit);

    private:
        // TODO: implement log.txt
    };

    class Time
    {
    public:
        static void Update();
        static double GetDeltaTime();
        static int GetFps();

    private:
        static std::chrono::time_point<std::chrono::high_resolution_clock> last_;
        static double deltaTime_;
        static int fps_;
    };
}

#endif //CGFIRE_UTIL_HPP
