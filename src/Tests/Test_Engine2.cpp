//
// Created by JS on 26/06/2021.
//

#include "engine/Util.hpp"
#include "engine/Window.hpp"
#include "engine/Camera.hpp"
#include "engine/model/Model.hpp"
#include <glm/gtx/transform.hpp>
#include "engine/prefab/SimplePointLight.hpp"
#include "engine/input/Input.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Spline3D.hpp"
#include "engine/prefab/BackgroundTerrain.hpp"

void DeleteRemainingResources()
{
    en::GLShader::DeleteAll();
    en::GLProgram::DeleteAll();
}

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

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
    en::Spline3DRenderable splineRenderable(&spline);
    en::Spline3D::Iterator iterator = { 0, 0.0f };

    en::Model backpackModel("backpack/backpack.obj", true);

    en::Model floorModel("cube.obj", true);
    floorModel.t_ = glm::translate(glm::vec3(0.0f, -5.0f, 0.0f)) * glm::scale(glm::vec3(256.0f, 1.0f, 256.0f));

    en::Model dragonModel("dragon.obj", false);
    dragonModel.t_ = glm::translate(glm::vec3(0.0f, 0.0f, 20.0f));

    en::Model reflectModel("hd_sphere.obj", false);
    reflectModel.t_ = glm::translate(glm::vec3(0.0f, 0.0, -8.0f));

    en::Model testModel("cube.obj", false);
    testModel.t_ = glm::translate(glm::vec3(64.0f, 10.0f, 0.0f)) * glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));

    en::Model splineFollowModel("cube.obj", false);

    // Lights
    en::DirLight dirLight(
            glm::vec3(0.3f, -1.0f, 1.0f),
            glm::vec3(0.5f),
            4096,
            4096);

    en::SimplePointLight pointLight(glm::vec3(1.0f, 1.0f, 1.0f), 200.0f, 512, 512);
    pointLight.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));
    std::vector<const en::PointLight*> pointLights = { (const en::PointLight*)&pointLight };

    // Skybox
    en::GLSkyboxTex skyboxTex("CGFire/skybox1", ".png", false);

    // Background Terrain
    en::BackgroundTerrain terrain(100, 20.0f, -4.0f, 128.0f, 10.0f);

    // Scene
    en::Log::Info("Creating SceneRenderer");

    en::SceneRenderer sceneRenderer(window.GetWidth(), window.GetHeight(), true, true);

    sceneRenderer.SetDirLight(&dirLight);
    sceneRenderer.AddPointLight(&pointLight);

    sceneRenderer.AddStandardRenderObj(&backpackModel);
    //sceneRenderer.AddStandardRenderObj(&floorModel);
    sceneRenderer.AddStandardRenderObj(&terrain);
    sceneRenderer.AddStandardRenderObj(&dragonModel);
    sceneRenderer.AddStandardRenderObj(&testModel);
    //sceneRenderer.AddStandardRenderObj(&splineFollowModel);

    sceneRenderer.AddFixedColorRenderObj(&pointLight);

    sceneRenderer.AddReflectiveRenderObj(&reflectModel, 2.0f);

    sceneRenderer.AddSplineRenderObj(&splineRenderable);

    sceneRenderer.SetSkyboxTex(&skyboxTex);

    // Main loop
    en::Log::Info("Staring main loop");
    while (window.IsOpen())
    {
        //en::Log::Info("Iteration");

        // Setup
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float) en::Time::GetDeltaTime();
        sceneRenderer.Resize(window.GetWidth(), window.GetHeight()); // TODO: maybe something more performant

        en::Input::HandleUserCamInput(&window, &cam, deltaTime);

        // Physics
        pointLight.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight.t_;
        backpackModel.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        dragonModel.t_ = dragonModel.t_ * glm::rotate(deltaTime * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        reflectModel.t_ = glm::rotate(deltaTime * 0.4f, glm::vec3(0.0f, 1.0f, 0.0f)) * reflectModel.t_;
        //splineFollowModel.t_ = glm::translate(spline.IterateRelative(&iterator, deltaTime * 0.5f));

        // Render
        cam.SetAspectRatio(window.GetAspectRatio());
        sceneRenderer.Render(&window, &cam);
    }

    DeleteRemainingResources();

    return 0;
}
