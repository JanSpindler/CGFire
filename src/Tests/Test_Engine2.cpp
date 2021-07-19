//
// Created by JS on 26/06/2021.
//

#include "engine/Util.hpp"
#include "engine/Window.hpp"
#include "engine/Camera.hpp"
#include "engine/model/Model.hpp"
#include <glm/gtx/transform.hpp>
#include "engine/render/Light.hpp"
#include "engine/prefab/SimplePointLight.hpp"
#include "engine/input/Input.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Spline3D.hpp"

#include <framework/imgui_util.hpp>
#include <GLFW/glfw3.h>



void DeleteRemainingResources()
{
    en::GLShader::DeleteAll();
    en::GLProgram::DeleteAll();
}

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");
    init_imgui(window.GetHandle());

    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -20.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            glm::radians(60.f),
            0.01f,
            1000.0f);

    // Models
    std::vector<glm::vec3> splinePoints = {
            { 5.0f, 10.0f, 5.0f },
            { -5.0f, 0.0f, 5.0f },
            { -5.0f, 5.0f, -5.0f },
            { 5.0f, 0.0f, -5.0f }
    };
    en::Spline3D spline(splinePoints, false, 32, en::Spline3D::TYPE_NATURAL_CUBIC);

    en::Model floorModel("cube.obj", true, "floor");
    floorModel.Position = glm::vec3(0.0f, -5.0f, 0.0f);
    floorModel.Scaling = glm::vec3(50.0f, 1.0f, 50.0f);

    en::Model houseModel("scene/house/Edificio.dae", false, "house");
    houseModel.Position =glm::vec3(40.0f, 0.0f, 20.0f);

    en::Model roboModel("vampire/Jumping_Down.dae", false, "rob");
    roboModel.Position =glm::vec3(10.0f, 0.0f, 40.0f);

    en::Model reflectModel("hd_sphere.obj", false, "reflect sphere");
    reflectModel.Position = glm::vec3(0.0f, 0.0, -8.0f);

    // Lights
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(0.5f));

    en::SimplePointLight pointLight(glm::vec3(1.0f, 1.0f, 1.0f), 200.0f, "Simple Point Light");
    pointLight.Position = glm::vec3(0.0f, 10.0f, 15.0f);
    std::vector<const en::PointLight*> pointLights = { (const en::PointLight*)&pointLight };

    // Skybox
    en::GLSkyboxTex skyboxTex("CGFire/skybox1", ".png", false);

    // Scene
    en::Log::Info("Creating SceneRenderer");

    en::SceneRenderer sceneRenderer(window.GetWidth(), window.GetHeight());

    sceneRenderer.SetDirLight(&dirLight);
    sceneRenderer.AddPointLight(&pointLight);

    sceneRenderer.AddStandardRenderObj(&floorModel);
    sceneRenderer.AddStandardRenderObj(&houseModel);
    sceneRenderer.AddStandardRenderObj(&roboModel);

    sceneRenderer.AddFixedColorRenderObj(&pointLight);

    sceneRenderer.AddReflectiveRenderObj(&reflectModel);

    sceneRenderer.AddSplineRenderObj(&spline);

    sceneRenderer.SetSkyboxTex(&skyboxTex);

    // Main loop
    en::Log::Info("Staring main loop");
    while (window.IsOpen())
    {
        // Setup
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float) en::Time::GetDeltaTime();
        sceneRenderer.Resize(window.GetWidth(), window.GetHeight()); // TODO: maybe something more performant

        en::Input::HandleUserCamInput(&window, &cam, deltaTime);

        // Physics
        pointLight.RotationAngle += deltaTime * -0.5f;
        reflectModel.RotationAngle += deltaTime * 0.4f;

        //UI
        bool renderImGui = !en::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (renderImGui) {
            imgui_new_frame(600, 400);
            sceneRenderer.OnImGuiRender();
        }

        // Render
        cam.SetAspectRatio(window.GetAspectRatio());
        sceneRenderer.Render(&window, &cam);

        if (renderImGui)
            imgui_render();
    }

    DeleteRemainingResources();
    cleanup_imgui();
    return 0;
}
