//
//Created by vincent on 10.07.2021.
//

#pragma once
#include <string>

namespace util{

    inline bool has_suffix(const std::string &str, std::string suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
}