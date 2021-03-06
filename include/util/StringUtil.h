//
//Created by vincent on 10.07.2021.
//

#pragma once
#include <string>
#include <sstream>

namespace util{

    inline bool HasSuffix(const std::string &str, std::string suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    inline std::string GetFloatPrecisionFixedString(float f, unsigned int precision){
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << f;
        return stream.str();
    }

    // https://stackoverflow.com/a/3418285
    inline void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
}