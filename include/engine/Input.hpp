//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_INPUT_HPP
#define CGFIRE_INPUT_HPP

#include <glm/glm.hpp>

struct GLFWwindow;

namespace en
{
    class Input
    {
    public:
        static void Init(GLFWwindow* windowHandle);
        static bool IsKeyPressed(int keycode);
        static bool IsMouseButtonPressed(int button);
        static glm::vec2 GetMousePos();

    private:
        static GLFWwindow* windowHandle_;

        static int KeycodeToGLFW(int keycode);
    };
}

#endif //CGFIRE_INPUT_HPP
