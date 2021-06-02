//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <chrono>
#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "util/OpenGLDebug.h"
#include "particle/ParticleSystem.h"
#include "particle/Fire.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/GLShader.hpp"
#include "engine/Model.hpp"
#include "engine/Light.hpp"
#include "engine/Input.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


std::chrono::time_point<std::chrono::system_clock> start_time;

void resizeCallback(GLFWwindow* window, int width, int height);


int main(int, char* argv[]) {

    en::Window window(800, 600, "CGFire");

    init_imgui(window.GetHandle());

    util::EnableGLDebugging();

    float fov = glm::radians(60.f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -20.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            fov,
            nearPlane,
            farPlane);


    // Particle Test
    using namespace particle;
    ParticleSystem particleSystemFire(10000, cam);
    FireCreator fireCreator(particleSystemFire);
    fireCreator.createFlame(glm::vec3(0.5f, 0.5f, 0.5f), 5);

    auto timeStart = std::chrono::steady_clock::now();


    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();

        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();


        // Mouse input handling
        bool mouseRightPressed = en::Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        window.EnableCursor(!mouseRightPressed);
        if (mouseRightPressed)
        {
            glm::vec2 mouseMove = 0.005f * -en::Input::GetMouseMove();
            cam.RotateViewDir(mouseMove.x, mouseMove.y);
        }

        // Keyboard input handling
        glm::vec3 camMove(0.0f, 0.0f, 0.0f);
        float camMoveSpeed = 2.0f * deltaTime;
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
        cam.Move(camMove);

        // Rendering
        cam.SetAspectRatio(window.GetAspectRatio());



        auto timeEnd = std::chrono::steady_clock::now();
        //calc difference in seconds
        float difference = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>
                (timeEnd - timeStart).count()) / 1000000.f;
        timeStart = timeEnd;


        fireCreator.onUpdate(difference);
        particleSystemFire.OnUpdate(difference);



        particleSystemFire.OnRender();




        imgui_render();
//        glfwSwapBuffers(window);
//        glfwPollEvents();
    }
//
    cleanup_imgui();
    //glfwTerminate();
}

void resizeCallback(GLFWwindow*, int width, int height)
{
    // set new width and height as viewport size
    glViewport(0, 0, width, height);
    //Error:
    //proj_matrix = glm::perspective(FOV, static_cast<float>(width) / static_cast<float>(height), NEAR_VALUE, FAR_VALUE);
}