#pragma once

#include <random>
namespace util {
    namespace Random {
        /*Generates a uniform random float between 0 and 1*/
        static float Float() {
            static std::mt19937 s_RandomEngine(std::random_device{}());
            static std::uniform_real_distribution<> s_Distribution;

            return s_Distribution(s_RandomEngine);
        }

    };
}