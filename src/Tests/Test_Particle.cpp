//
//Created by vincent on 25.05.2021.
// Testet das ParticleSystem

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "particle/ParticleSystem.h"
#include "particle/Fire.h"
#include "particle/Water.h"
#include "particle/Smoke.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "util/UserCamMovement.h"
#include <project/ObjectManager.h>



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

    sound::SoundManager soundManager(cam);

    // Particles
    using namespace particle;

    //Fire
    ParticleSystem particleSystemAdditive(3000, cam, true);
    FireCreator fireCreator(particleSystemAdditive, soundManager);

    fireCreator.startFlame(std::make_shared<Flame>
            ("Fire Uno",
                    glm::vec3(0.f, 0.f, 0.f),
             glm::vec3(1.f, 0.f, 1.f),
             30,
             5.f,
             10.f,
             1.f,
             0.2f));

    ParticleSystem particleSystemNoAdditive(7000, cam, false);
    SmokeCreator smokeCreator(particleSystemNoAdditive);

    const en::GLShader* fixedColorVert = en::GLShader::Load("CGFire/fixed_color.vert");
    const en::GLShader* fixedColorFrag = en::GLShader::Load("CGFire/fixed_color.frag");
    const en::GLProgram* fixedColorProgram = en::GLProgram::Load(fixedColorVert, nullptr, fixedColorFrag);
    std::vector<glm::vec3> splinePoints = {
            { 0.0f, 2.0f, 0.0f },
            { 5.0f, 5.0f, 0.0f },
            { 10.0f, 10.0f, -5.0f },
            { 35.0f, 30.0f, -15.0f }
    };
    std::shared_ptr<en::Spline3D> spline = std::make_shared<en::Spline3D>(splinePoints,
                                           false,
                                           40,
                                           en::Spline3D::TYPE_CATMULL_ROM);

    smokeCreator.startSmokeStream(std::make_shared<SmokeStream>
                                   ("Rauch Uno",
                                    spline,
                                    glm::vec3(1.f, 0.f, 1.f)));

    //Water
    WaterCreator waterCreator(particleSystemNoAdditive, soundManager);

    waterCreator.startWaterJet(std::make_shared<WaterJet>(
            "Water Uno",
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

    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();
        util::HandleUserCamMovement(window, cam, deltaTime);

        sf::Listener::setPosition(cam.GetPos().x, cam.GetPos().y, cam.GetPos().z);
        sf::Listener::setDirection(cam.GetViewDir().x, cam.GetViewDir().y, cam.GetViewDir().z);

        //Updates
        fireCreator.onUpdate(deltaTime);
        particleSystemAdditive.OnUpdate(deltaTime);
        waterCreator.onUpdate(deltaTime);
        smokeCreator.onUpdate(deltaTime);
        particleSystemNoAdditive.OnUpdate(deltaTime);


        //UI
        bool renderImGui = !en::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (renderImGui) {
            imgui_new_frame(600, 400);
            fireCreator.onImGuiRender();
            waterCreator.onImGuiRender();
            smokeCreator.onImGuiRender();
        }

        //Rendering
        cam.SetAspectRatio(window.GetAspectRatio());

        window.UseViewport();

        particleSystemAdditive.OnRender();
        particleSystemNoAdditive.OnRender();


        if (renderImGui)
            imgui_render();

        auto viewMat = cam.GetViewMat();
        auto projMat = cam.GetProjMat();

        // Test render spline
        fixedColorProgram->Use();
        fixedColorProgram->SetUniformMat4("view_mat", false, &viewMat[0][0]);
        fixedColorProgram->SetUniformMat4("proj_mat", false, &projMat[0][0]);
        spline->Render(fixedColorProgram);
    }

    cleanup_imgui();
    return 0;
}