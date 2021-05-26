//
// Created by JS on 25/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/Input.hpp"

namespace en
{
    GLFWwindow* Input::windowHandle_ = nullptr;

    void Input::Init(GLFWwindow* windowHandle)
    {
        windowHandle_ = windowHandle;
    }

    bool Input::IsKeyPressed(int keycode)
    {
        int state = glfwGetKey(windowHandle_, KeycodeToGLFW(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        int state = glfwGetMouseButton(windowHandle_, button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePos()
    {
        double xpos;
        double ypos;
        glfwGetCursorPos(windowHandle_, &xpos, &ypos);
        return glm::vec2(xpos, ypos);
    }

    int Input::KeycodeToGLFW(int keycode)
    {
        return keycode;
    }
}
