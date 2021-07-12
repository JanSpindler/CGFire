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
}