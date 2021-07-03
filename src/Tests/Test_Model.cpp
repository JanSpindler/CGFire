//
// Created by JS on 26/06/2021.
//

#include "engine/gr_include.hpp"

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
#include "engine/config.hpp"

#include <framework/imgui_util.hpp>
#include <GLFW/glfw3.h>

#include <framework/mesh.hpp>


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

    std::string fileStr = MODEL_ROOT + "scene/house/Edificio.dae";
    std::vector<geometry> scene = loadScene(fileStr.c_str(), true);

    const en::GLShader* vert = en::GLShader::Load("CGFire/fixed_color.vert");
    const en::GLShader* frag = en::GLShader::Load("CGFire/fixed_color.frag");
    const en::GLProgram* program = en::GLProgram::Load(vert, nullptr, frag);

    // Main loop
    en::Log::Info("Staring main loop");
    while (window.IsOpen())
    {
        // Setup
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float) en::Time::GetDeltaTime();

        en::Input::HandleUserCamInput(&window, &cam, deltaTime);

        // Physics

        //UI
        bool renderImGui = !en::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (renderImGui) {
            imgui_new_frame(600, 400);
        }

        // Render
        window.UseViewport();
        cam.SetAspectRatio(window.GetAspectRatio());

        program->Use();
        program->SetUniformVec4f("diffuse_color", glm::vec4(1.0f));
        program->SetUniformMat4("proj_mat", false, &cam.GetProjMat()[0][0]);
        program->SetUniformMat4("view_mat", false, &cam.GetViewMat()[0][0]);

        for (geometry& geo : scene)
        {
            geo.bind();
            program->SetUniformMat4("model_mat", false, &geo.transform[0][0]);
            glDrawElements(GL_TRIANGLES, geo.vertex_count, GL_UNSIGNED_INT, nullptr);
            geo.release();
        }

        if (renderImGui)
            imgui_render();
    }

    DeleteRemainingResources();
    cleanup_imgui();
    return 0;
}
