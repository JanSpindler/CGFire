//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <chrono>
#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "particle/ParticleSystem.h"
#include "particle/Fire.h"
#include "particle/Water.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/render/GLShader.hpp"
#include "engine/model/Model.hpp"
#include "engine/render/Light.hpp"
#include "engine/input/Input.hpp"

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
    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -20.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            glm::radians(60.f),
            0.01f,
            1000.0f);


    // Particle Test
    using namespace particle;

    //Fire
    ParticleSystem particleSystemFire(10000, cam, true);
    FireCreator fireCreator(particleSystemFire);

    Flame flame1(glm::vec3(0.5f, 0.5f, 0.5f),
                 glm::vec3(1.f, 0.f, 1.f),
                 30.f,
                 15.f,
                 10.f,
                 1.f, 0.2f);
    fireCreator.startFlame(flame1);

    //TODO Time event für Flammen expiring

    //Water
    ParticleSystem particleSystemWater(10000, cam, false);
    WaterCreator waterCreator(particleSystemWater);
    auto waterJet1 = waterCreator.createWaterJet(glm::vec3(2.f, 0.f, 1.f),
                                                glm::vec3(20.f, 10.f, 0.f),
                                                60);

    while (window.IsOpen())
    {
        window.Update();
        window.UseViewport();
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

        // Window viewport
        window.UseViewport();

        // UI
        imgui_new_frame(1200, 800);
        ImGui::Begin("Fire1");
        ImGui::SliderFloat3("Position", &flame1.Position.x, -10, 10);
        ImGui::SliderFloat3("PositionVariation", &flame1.PositionVariation.x, 0, 10);
        ImGui::SliderInt("ParticlesPerEmit", &flame1.ParticlesPerEmit, 0, 100);
        ImGui::SliderFloat("ParticleLifeTime", &flame1.ParticleLifeTime, 0, 10);
        ImGui::SliderFloat("ParticleLifeTimeVariation", &flame1.ParticleLifeTimeVariation, 0, 2);
        ImGui::End();

        ImGui::Begin("WaterJet1");
        ImGui::SliderFloat3("Position", &waterJet1->Position.x, -10, 10);
        ImGui::SliderFloat3("StartGradient", &waterJet1->StartGradient.x, 0, 100);
        ImGui::SliderInt("ParticlesPerSecond", &waterJet1->ParticlesPerSecond, 0, 500);
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