//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "particle/ParticleSystem.h"
#include "particle/Fire.h"
#include "particle/Water.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "util/UserCamMovement.h"

#include "project/SceneManager.h"


int main(int, char* argv[]) {
    en::Window window(1200, 800, "CGFire");
    init_imgui(window.GetHandle());

    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -20.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            glm::radians(60.f),
            0.01f,
            1000.0f);

    // Particle Systems
    using namespace particle;

    //Fire
    ParticleSystem particleSystemFire(10000, cam, true);
    FireCreator fireCreator(particleSystemFire);

    //Water
    ParticleSystem particleSystemWater(10000, cam, false);
    WaterCreator waterCreator(particleSystemWater);

    //Create the Scene
    scene::SceneManager Scene(cam, fireCreator, waterCreator);
    Scene.restart();

    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();
        util::HandleUserCamMovement(window, cam, deltaTime);


        //Updates
        fireCreator.onUpdate(deltaTime);
        particleSystemFire.OnUpdate(deltaTime);
        waterCreator.onUpdate(deltaTime);
        particleSystemWater.OnUpdate(deltaTime);

        Scene.onUpdate(deltaTime);

        //UI
        bool renderImGui = !en::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (renderImGui) {
            imgui_new_frame(600, 400);
            if (ImGui::Begin("Fire")) {
                fireCreator.onImGuiRender();
                ImGui::End();
            }
            if (ImGui::Begin("Water")) {
                waterCreator.onImGuiRender();
                ImGui::End();
            }
            Scene.onImGuiRender();
        }

        //Rendering
        cam.SetAspectRatio(window.GetAspectRatio());

        window.UseViewport();
        particleSystemFire.OnRender();
        particleSystemWater.OnRender();

        if (renderImGui)
            imgui_render();
    }

    cleanup_imgui();
    exit(0); //ImGui Bug sorgt sonst für Fehler
}