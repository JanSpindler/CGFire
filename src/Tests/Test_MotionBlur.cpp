//
// Created by Nika on 11/06/2021.
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
#include "MotionBlur/motionblur.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/string_cast.hpp>


#include "util/UserCamMovement.h"

int main()
{
    en::Window window(800, 600, "CGFire");

    en::Camera cam(
            glm::vec3(0.0f,0.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            glm::radians(60.f),
            0.01f,
            1000.0f);

    glm::mat4 projMat;
    glm::mat4 viewMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    const en::GLShader* vertShader = en::GLShader::Load("skeletalblur.vert");
    const en::GLShader* fragShader = en::GLShader::Load("skeletalblur.frag");
    const en::GLProgram* program = en::GLProgram::Load(vertShader, nullptr, fragShader);

    en::Model vampiremodel("vampire/dancing_vampire.dae", true);
    vampiremodel.SetName("vampire");
    en::Animation animation("vampire/dancing_vampire.dae", &vampiremodel);
    en::Animator animator(&animation);

    en::motionblur motionblur(window.GetWidth(), window.GetHeight());
    int width = window.GetWidth();
    int height = window.GetHeight();
    const en::GLProgram* renderprog = motionblur.makerenderprog();
    animator.UpdateAnim(0.0f);
    motionblur.currenttransforms = animator.getfinalbonetransforms();
    motionblur.prevprojviewmodelmat =  projMat*viewMat*vampiremodel.GetTransform();

    program->Use();
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(1.0f));
    dirLight.Use(program);

    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();
        util::HandleUserCamMovement(window, cam, deltaTime);

        animator.UpdateAnim(deltaTime);

//        if (glCheckNamedFramebufferStatus(motionblur.ssaofbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//            printf("Incomplete FBO!");
//            std::terminate();
//        }

        if(width != window.GetWidth()||height!= window.GetHeight()){
            motionblur.build_framebuffer(window.GetWidth(), window.GetHeight());
            width = window.GetWidth();
            height = window.GetHeight();
        }

        motionblur.firstpasssetup(window.GetWidth(), window.GetHeight());


        // Rendering
        cam.SetAspectRatio(window.GetAspectRatio());
        window.UseViewport();

        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();
        program->Use();
        program->SetUniformMat4("view_mat", false, &viewMat[0][0]);
        program->SetUniformMat4("proj_mat", false, &projMat[0][0]);
        program->SetUniformVec3f("cam_pos", cam.GetPos());

        auto transforms = animator.getfinalbonetransforms();

        motionblur.updatetransforms(transforms);
        motionblur.addskeletalarrays(program);
        motionblur.addprevprojviewmodelmat(program);

        //vampireObj.t_ *= glm::rotate(deltaTime * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        //en::Log::Info(glm::to_string(motionblur.prevprojviewmodelmat)+"\n"+glm::to_string(motionblur.prevtransforms[0])+"\n"+glm::to_string(motionblur.currenttransforms[0]));
        vampiremodel.Render(program);

        /****Aufruf geht nimmer:****/
        //motionblur.doblur(renderprog);
        motionblur.prevprojviewmodelmat = projMat*viewMat*vampiremodel.GetTransform();
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
