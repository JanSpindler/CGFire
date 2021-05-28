//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_INPUT_HPP
#define CGFIRE_INPUT_HPP

#include <glm/glm.hpp>
#include <unordered_map>
#include "keycode.hpp"
#include "mousecode.hpp"

struct GLFWwindow;

namespace en
{
    class Input
    {
    public:
        static void Init(GLFWwindow* windowHandle);
        static void Update();
        static bool IsKeyPressed(int keycode);
        static bool IsMouseButtonPressed(int button);
        static glm::vec2 GetMousePos();
        static glm::vec2 GetMouseMove();

    private:
        static GLFWwindow* windowHandle_;
        static glm::vec2 mousePos_;
        static glm::vec2 mouseMove_;
    };
}

#endif //CGFIRE_INPUT_HPP
