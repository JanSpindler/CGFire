//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

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



    //Create the Scene
    scene::SceneManager Scene(cam, window);
    Scene.restart();

    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();
        util::HandleUserCamMovement(window, cam, deltaTime);


        //Updates
        Scene.onUpdate(deltaTime);

        //UI
        bool renderImGui = !en::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (renderImGui) {
            imgui_new_frame(600, 400);
            Scene.onImGuiRender();
        }

        //Rendering
        cam.SetAspectRatio(window.GetAspectRatio());
        window.UseViewport();
        Scene.OnRender();

        if (renderImGui)
            imgui_render();
    }

    cleanup_imgui();
    exit(0); //ImGui Bug sorgt sonst für Fehler
}