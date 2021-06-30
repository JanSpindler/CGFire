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

#include "particle/ParticleSystem.h"
#include "particle/Fire.h"
#include "particle/Water.h"
#include "particle/Smoke.h"

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
    en::Spline3DRenderable splineRenderable(&spline);

    en::Model backpackModel("backpack/backpack.obj", true);

    en::Model floorModel("cube.obj", true);
    floorModel.t_ = glm::translate(glm::vec3(0.0f, -5.0f, 0.0f)) * glm::scale(glm::vec3(50.0f, 1.0f, 50.0f));

    en::Model dragonModel("dragon.obj", false);
    dragonModel.t_ = glm::translate(glm::vec3(0.0f, 0.0f, 20.0f));

    en::Model reflectModel("hd_sphere.obj", false);
    reflectModel.t_ = glm::translate(glm::vec3(0.0f, 0.0, -8.0f));

    // Lights
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(0.5f));

    en::SimplePointLight pointLight(glm::vec3(1.0f, 1.0f, 1.0f), 200.0f);
    pointLight.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));
    std::vector<const en::PointLight*> pointLights = { (const en::PointLight*)&pointLight };

    // Skybox
    en::GLSkyboxTex skyboxTex("CGFire/skybox1", ".png", false);

    // Scene
    en::Log::Info("Creating SceneRenderer");

    en::SceneRenderer sceneRenderer(window.GetWidth(), window.GetHeight());

    sceneRenderer.SetDirLight(&dirLight);
    sceneRenderer.AddPointLight(&pointLight);

    sceneRenderer.AddStandardRenderObj(&backpackModel);
    sceneRenderer.AddStandardRenderObj(&floorModel);
    sceneRenderer.AddStandardRenderObj(&dragonModel);

    sceneRenderer.AddFixedColorRenderObj(&pointLight);

    sceneRenderer.AddReflectiveRenderObj(&reflectModel);

    sceneRenderer.AddSplineRenderObj(&splineRenderable);

    sceneRenderer.SetSkyboxTex(&skyboxTex);

// Particles
    using namespace particle;

    //Fire
    ParticleSystem particleSystemFire(3000, cam, true);
    FireCreator fireCreator(particleSystemFire);

    fireCreator.startFlame(std::make_shared<Flame>
                                   (glm::vec3(0.f, 0.f, 0.f),
                                    glm::vec3(1.f, 0.f, 1.f),
                                    30,
                                    5.f,
                                    10.f,
                                    1.f,
                                    0.2f));

    ParticleSystem particleSystemSmoke(4000, cam, false);
    SmokeCreator smokeCreator(particleSystemSmoke);

    const en::GLShader* fixedColorVert = en::GLShader::Load("CGFire/fixed_color.vert");
    const en::GLShader* fixedColorFrag = en::GLShader::Load("CGFire/fixed_color.frag");
    const en::GLProgram* fixedColorProgram = en::GLProgram::Load(fixedColorVert, nullptr, fixedColorFrag);
    std::vector<glm::vec3> splinePoints2 = {
            { 0.0f, 2.0f, 0.0f },
            { 5.0f, 5.0f, 0.0f },
            { 10.0f, 10.0f, -5.0f },
            { 35.0f, 30.0f, -15.0f }
    };
    std::shared_ptr<en::Spline3D> spline2 = std::make_shared<en::Spline3D>(splinePoints2, false, 40, en::Spline3D::TYPE_NATURAL_CUBIC);
    en::Spline3DRenderable splineRenderable2(spline2.get());
    sceneRenderer.AddSplineRenderObj(&splineRenderable2);

    smokeCreator.startSmokeStream(std::make_shared<SmokeStream>
                                          (spline2,
                                           glm::vec3(0.5f, 0.5f, 0.5f),
                                           glm::vec3(1.f, 0.f, 1.f)));

    //Water
    ParticleSystem particleSystemWater(3000, cam, false);
    WaterCreator waterCreator(particleSystemWater);

    waterCreator.startWaterJet(std::make_shared<WaterJet>(
            glm::vec3(2.f, 0.f, 1.f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.f, 0.5f, 0.f),
            100.f,
            0.1f,
            500,
            10.f,
            2.f,
            5.f,
            0.2f));

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


        //Updates
        fireCreator.onUpdate(deltaTime);
        particleSystemFire.OnUpdate(deltaTime);
        waterCreator.onUpdate(deltaTime);
        particleSystemWater.OnUpdate(deltaTime);
        smokeCreator.onUpdate(deltaTime);
        particleSystemSmoke.OnUpdate(deltaTime);



        // Physics
        pointLight.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight.t_;
        backpackModel.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        dragonModel.t_ = dragonModel.t_ * glm::rotate(deltaTime * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        reflectModel.t_ = glm::rotate(deltaTime * 0.4f, glm::vec3(0.0f, 1.0f, 0.0f)) * reflectModel.t_;

        // Render
        cam.SetAspectRatio(window.GetAspectRatio());
        sceneRenderer.Render(&window, &cam);

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
            if (ImGui::Begin("Smoke")) {
                smokeCreator.onImGuiRender();
                ImGui::End();
            }
        }

        particleSystemWater.OnRender();
        particleSystemSmoke.OnRender();
        particleSystemFire.OnRender();

        if (renderImGui)
            imgui_render();
    }

    DeleteRemainingResources();


    cleanup_imgui();
    exit(0); //ImGui Bug sorgt sonst für Fehler
    return 0;
}
