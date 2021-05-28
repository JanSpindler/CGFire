//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <chrono>
#include <glm/gtx/transform.hpp>
#include "framework/common.hpp"
#include "framework/shader.hpp"
#include "framework/buffer.hpp"
#include "framework/camera.hpp"
#include "framework/mesh.hpp"

#include <framework/imgui_util.hpp>

#include "util/OpenGLDebug.h"
#include "particle/ParticleSystem.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


const int WINDOW_WIDTH =  800;
const int WINDOW_HEIGHT = 800;
const float FOV = 45.f;
const float NEAR_VALUE = 0.1f;
const float FAR_VALUE = 100.f;
const float SUN_EARTH_DISTANCE = 5.f;
const float EARTH_MOON_DISTANCE = 2.f;


std::chrono::time_point<std::chrono::system_clock> start_time;

void resizeCallback(GLFWwindow* window, int width, int height);


int main(int, char* argv[]) {
    GLFWwindow* window = initOpenGL(WINDOW_WIDTH, WINDOW_HEIGHT, argv[0]);
    glfwSetFramebufferSizeCallback(window, resizeCallback);

    util::EnableGLDebugging();

//    camera cam(window);

    init_imgui(window);

    glEnable(GL_DEPTH_TEST);

    // Particle Test
    ParticleSystem particleSystem;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Soll Feuer darstellen
    ParticleProps particleProps;
    particleProps.ColorBegin = { 210 / 255.0f, 200 / 255.0f, 0 / 255.0f, 1.0f };
    particleProps.ColorEnd = { 250 / 255.0f, 0 / 255.0f, 0 / 255.0f, 0.3f };
    particleProps.SizeBegin = 0.09f;
    particleProps.SizeVariation = 0.09f;
    particleProps.SizeEnd = 0.0f;
    particleProps.LifeTime = 0.25f;
    particleProps.LifeTimeVariation = 0.06f;
    particleProps.Velocity = { 0.0f, 10.0f, 0.0f };
    particleProps.VelocityVariation = { 6.0f, 0.0f, 0.0f };
    particleProps.Position = { -0.6f, -0.6f, 0.0f };
    particleProps.PositionVariation = { 0.05f, 0.05f, 2.f };
    auto timePassed = std::chrono::steady_clock::now();
    float emitTimer = 0;

    glm::mat4 view_proj_matrix;
    auto view_matrix = glm::identity<glm::mat4>();


    while (glfwWindowShouldClose(window) == false) {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        auto timeNow = std::chrono::steady_clock::now();
        float difference =(std::chrono::duration_cast<std::chrono::microseconds>(timeNow - timePassed).count()) /1000000.f;
        timePassed = timeNow;
        emitTimer += difference;
        if (emitTimer > 0.1f) {
            float xPos = particleProps.Position.x;
            for (int i = 0; i < 10; ++i) {
                particleProps.Position.x += i * particleProps.SizeBegin*0.1;
                particleSystem.Emit(particleProps);
            }
            particleProps.Position.x = xPos;
            emitTimer = 0.f;
        }

        particleSystem.OnUpdate(difference);
        particleSystem.OnRender(view_matrix);



        // UI
        imgui_new_frame(400, 200);
        ImGui::Begin("Particles");
        ImGui::ColorEdit4("ColorBegin", &particleProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &particleProps.ColorEnd.x);
        ImGui::SliderFloat("SizeBegin", &particleProps.SizeBegin, 0, 10);
        ImGui::SliderFloat("SizeVariation", &particleProps.SizeVariation, 0, 10);
        ImGui::SliderFloat("SizeEnd", &particleProps.SizeEnd, 0, 10);
        ImGui::SliderFloat("LifeTime", &particleProps.LifeTime, 0, 10);
        ImGui::SliderFloat3("Velocity", &particleProps.Velocity.x, -10, 10);
        ImGui::SliderFloat3("VelocityVariation", &particleProps.VelocityVariation.x, 0, 10);
        ImGui::SliderFloat3("Position", &particleProps.Position.x, -10, 10);
        ImGui::End();

        imgui_render();
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    cleanup_imgui();
    glfwTerminate();
}

void resizeCallback(GLFWwindow*, int width, int height)
{
    // set new width and height as viewport size
    glViewport(0, 0, width, height);
    //Error:
    //proj_matrix = glm::perspective(FOV, static_cast<float>(width) / static_cast<float>(height), NEAR_VALUE, FAR_VALUE);
}