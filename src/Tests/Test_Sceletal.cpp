//
// Created by Nika on 03/06/2021.
//

#include "RiggingAndBlending/animator.hpp"
#include "RiggingAndBlending/animation.hpp"
#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/string_cast.hpp>

#include <glm/gtx/transform.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/render/GLShader.hpp"
#include "engine/model/Model.hpp"
#include "engine/render/Light.hpp"
#include "engine/input/Input.hpp"
#include "framework/common.hpp"
#include "framework/shader.hpp"
#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/raytracer.hpp"

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

    float fov = glm::radians(60.f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    en::Camera cam(
            glm::vec3(0.0f, 1.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            fov,
            nearPlane,
            farPlane);

    glm::mat4 projMat;
    glm::mat4 viewMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    en::GLShader vertShader("sceletal.vert", en::GLShader::Type::VERTEX);
    en::GLShader fragShader("sceletal.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram program(&vertShader, nullptr, &fragShader);

    en::Model vampiremodel("vampire/dancing_vampire.dae", true);
    en::RenderObj vampireObj = { &vampiremodel };
    en::Animation animation("vampire/dancing_vampire.dae", &vampiremodel);
    en::Animator animator(&animation);

    //en::Model bobmodel("boblampmodel/boblampclean.md5mesh", true);
    //en::RenderObj bobObj = { &bobmodel };
    //en::Animation animation("boblampmodel/boblampclean.md5anim", &bobmodel);
    //en::Animator animator(&animation);

    program.Use();
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(1.0f));
    dirLight.Use(&program);

    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        auto deltaTime = (float)en::Time::GetDeltaTime();

        // Use window viewport
        window.UseViewport();

        animator.UpdateAnim(deltaTime);
        // Rendering
        cam.SetAspectRatio(window.GetAspectRatio());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();
        program.Use();
        program.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        program.SetUniformMat4("proj_mat", false, &projMat[0][0]);
        program.SetUniformVec3f("cam_pos", cam.GetPos());

        auto transforms = animator.getfinalbonetransforms();
        for (int i = 0; i < transforms.size(); ++i) {
            program.SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(transforms[i]));
            //en::Log::Info(glm::to_string(transforms[i]));
        };

        vampireObj.Render(&program);
        //bobObj.Render(&program);


    }

    en::Log::Info("Ending CGFire");
    return 0;
}
