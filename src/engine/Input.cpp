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

    void Input::HandleUserCamInput(Window* window, Camera* cam, float deltaTime)
    {
        // Mouse input handling
        bool mouseRightPressed = en::Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        window->EnableCursor(!mouseRightPressed);
        if (mouseRightPressed)
        {
            glm::vec2 mouseMove = 0.005f * -en::Input::GetMouseMove();
            cam->RotateViewDir(mouseMove.x, mouseMove.y);
        }

        // Keyboard input handling
        glm::vec3 camMove(0.0f, 0.0f, 0.0f);
        float camMoveSpeed = 20.0f * deltaTime;
        bool frontPressed = en::Input::IsKeyPressed(KEY_W);
        bool backPressed = en::Input::IsKeyPressed(KEY_S);
        bool leftPressed = en::Input::IsKeyPressed(KEY_A);
        bool rightPressed = en::Input::IsKeyPressed(KEY_D);
        bool upPressed = en::Input::IsKeyPressed(KEY_SPACE);
        bool downPressed = en::Input::IsKeyPressed(KEY_C);
        if (frontPressed && !backPressed)
            camMove.z = camMoveSpeed;
        else if (backPressed && !frontPressed)
            camMove.z = -camMoveSpeed;
        if (rightPressed && !leftPressed)
            camMove.x = camMoveSpeed;
        else if (leftPressed  && !rightPressed)
            camMove.x = -camMoveSpeed;
        if (upPressed && !downPressed)
            camMove.y = camMoveSpeed;
        else if (downPressed && !upPressed)
            camMove.y = -camMoveSpeed;

        if (camMove != glm::vec3(0.f, 0.f, 0.f))
            cam->Move(camMove);
    }
}
