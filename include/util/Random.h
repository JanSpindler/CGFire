#pragma once

#include <random>
namespace util {
    class Random {
    public:
        static void Init() {
            //Standard mersenne_twister_engine seeded with std::random_device()
            s_RandomEngine.seed(std::random_device()());
        }

        /*Generates a uniform random float between 0 and 1*/
        static float Float() {
            return (float) s_Distribution(s_RandomEngine) / (float) std::numeric_limits<uint32_t>::max();
        }

    private:
        static std::mt19937 s_RandomEngine;
        static std::uniform_int_distribution<> s_Distribution;
    };
}