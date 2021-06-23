#pragma once

#include <random>
namespace util {
    namespace Random {
        /*Generates a uniform random float between 0 and 1*/
        static float Float() {
            static std::mt19937 s_RandomEngine(std::random_device{}());
            static std::uniform_real_distribution<> s_Distribution;

            return static_cast<float>(s_Distribution(s_RandomEngine));
        }

        static uint32_t Uint32(uint32_t minValue, uint32_t maxValue){
            static std::mt19937 s_RandomEngine(std::random_device{}());
            static std::uniform_int_distribution<> s_Distribution;

            return minValue + s_Distribution(s_RandomEngine) % (maxValue-minValue + 1);
        }
    };
}