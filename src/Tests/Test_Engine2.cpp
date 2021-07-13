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

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

    en::SimplePointLight pointLight0(glm::vec3(1.0f, 0.0f, 0.0f), 200.0f, 512, 512);
    pointLight0.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));

    en::SimplePointLight pointLight1(glm::vec3(0.0f, 1.0f, 1.0f), 100.0f, 512, 512);
    pointLight1.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));

    // Skybox
    en::GLSkyboxTex skyboxTex("CGFire/skybox1", ".png", false);

    // Background Terrain
    en::BackgroundTerrain terrain(100, 20.0f, -4.0f, 1.0f, 128.0f, 0.0f);

    // Scene
    en::Log::Info("Creating SceneRenderer");

    en::SceneRenderer sceneRenderer(window.GetWidth(), window.GetHeight(), true, true);

    sceneRenderer.SetDirLight(&dirLight);
    sceneRenderer.AddPointLight(&pointLight0);
    sceneRenderer.AddPointLight(&pointLight1);

    sceneRenderer.AddStandardRenderObj(&backpackModel);
    sceneRenderer.AddStandardRenderObj(&terrain);
    sceneRenderer.AddStandardRenderObj(&dragonModel);
    sceneRenderer.AddStandardRenderObj(&testModel);
    sceneRenderer.AddStandardRenderObj(&splineFollowModel);

    sceneRenderer.AddFixedColorRenderObj(&pointLight0);
    sceneRenderer.AddFixedColorRenderObj(&pointLight1);

    sceneRenderer.AddReflectiveRenderObj(&reflectModel, 2.0f, 1.0f);

    sceneRenderer.AddSplineRenderObj(&splineRenderable);

    sceneRenderer.SetSkyboxTex(&skyboxTex);

    cam.TrackSpline(&spline);

    testModel.SetMeshMaterial(backpackModel.GetMeshMaterial(0), 0);

    // Main loop
    en::Log::Info("Staring main loop");
    while (window.IsOpen())
    {
        // Setup
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float) en::Time::GetDeltaTime();
        sceneRenderer.Resize(window.GetWidth(), window.GetHeight());

        en::Input::HandleUserCamInput(&window, &cam, deltaTime);
        
        // Physics
        pointLight0.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight0.t_;
        backpackModel.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        dragonModel.t_ = dragonModel.t_ * glm::rotate(deltaTime * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        reflectModel.t_ = glm::rotate(deltaTime * 0.4f, glm::vec3(0.0f, 1.0f, 0.0f)) * reflectModel.t_;

        glm::vec3 segmentDir = spline.GetSegmentDir(iterator.lastPoint);
        glm::mat4 rotMat = glm::orientation(segmentDir, glm::vec3(0.0f, 1.0f, 0.0f));
        splineFollowModel.t_ =
                glm::translate(spline.IterateRelative(&iterator, 10.0f * deltaTime)) * rotMat;

        cam.TrackStep(deltaTime * 2.0f, glm::vec3(0.0f));

        // Render
        cam.SetAspectRatio(window.GetAspectRatio());
        sceneRenderer.Render(&cam);
    }

    DeleteRemainingResources();

    return 0;
}
