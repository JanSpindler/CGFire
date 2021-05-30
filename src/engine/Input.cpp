//
// Created by JS on 25/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/Input/Input.hpp"

namespace en
{
    GLFWwindow* Input::windowHandle_ = nullptr;
    glm::vec2 Input::mousePos_ = glm::vec2(0.0f);
    glm::vec2 Input::mouseMove_ = glm::vec2(0.0f);

    void Input::Init(GLFWwindow* windowHandle)
    {
        windowHandle_ = windowHandle;
        Update();
    }

    void Input::Update()
    {
        double xpos;
        double ypos;
        glfwGetCursorPos(windowHandle_, &xpos, &ypos);
        glm::vec2 newMousePos = glm::vec2(xpos, ypos);

        mouseMove_ = newMousePos - mousePos_;
        mousePos_ = newMousePos;
    }

    bool Input::IsKeyPressed(int keycode)
    {
        int state = glfwGetKey(windowHandle_, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        int state = glfwGetMouseButton(windowHandle_, button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePos()
    {
        return mousePos_;
    }

    glm::vec2 Input::GetMouseMove()
    {
        return mouseMove_;
    }
}
