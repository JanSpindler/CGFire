//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <chrono>
#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "util/OpenGLDebug.h"
#include "particle/ParticleSystem.h"
#include "particle/Fire.h"
#include "particle/Water.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/GLShader.hpp"
#include "engine/Model.hpp"
#include "engine/Light.hpp"
#include "engine/Input.hpp"

glm::vec3 getCamMovement(float deltaTime){
    glm::vec3 camMove(0.0f, 0.0f, 0.0f);
    float camMoveSpeed = 6.0f * deltaTime;
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

    return camMove;
}

int main(int, char* argv[]) {

    en::Window window(1200, 800, "CGFire");

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
    ParticleSystem particleSystemFire(10000, cam, true);
    FireCreator fireCreator(particleSystemFire);
    auto fire1 = fireCreator.createFlame(glm::vec3(0.5f, 0.5f, 0.5f), 5);

    ParticleSystem particleSystemWater(10000, cam, false);
    WaterCreator waterCreator(particleSystemWater);
    auto waterJet1 = waterCreator.createWaterJet(glm::vec3(2.f, 0.f, 1.f),
                                                glm::vec3(20.f, 10.f, 0.f),
                                                60);


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
        cam.Move(getCamMovement(deltaTime));

        // Rendering
        cam.SetAspectRatio(window.GetAspectRatio());


        // UI
        imgui_new_frame(1200, 800);
        ImGui::Begin("Fire1");
        ImGui::SliderFloat3("Position", &fire1->Position.x, -10, 10);
        ImGui::SliderInt("ParticlesPerEmit", &fire1->ParticlesPerEmit, 0, 100);
        ImGui::End();

        ImGui::Begin("WaterJet1");
        ImGui::SliderFloat3("Position", &waterJet1->Position.x, -10, 10);
        ImGui::SliderFloat3("StartGradient", &waterJet1->StartGradient.x, 0, 50);
        ImGui::SliderInt("ParticlesPerSecond", &waterJet1->ParticlesPerSecond, 0, 1000);
        ImGui::End();



        fireCreator.onUpdate(deltaTime);
        waterCreator.onUpdate(deltaTime);
        particleSystemFire.OnUpdate(deltaTime);
        particleSystemWater.OnUpdate(deltaTime);


        particleSystemFire.OnRender();
        particleSystemWater.OnRender();

        imgui_render();
    }

    cleanup_imgui();
}