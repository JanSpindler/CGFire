

#include <glm/gtx/transform.hpp>

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/Render/GLShader.hpp"
#include "engine/Model/Model.hpp"
#include "engine/Render/Light.hpp"
#include "engine/Input/Input.hpp"
#include "engine/prefab/SimplePointLight.hpp"
#include "engine/Spline3D.hpp"

#include "util/UserCamMovement.h"

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

    // Shader
    en::GLShader fixedColorVert("CGFire/fixed_color.vert", en::GLShader::Type::VERTEX);
    en::GLShader fixedColorFrag("CGFire/fixed_color.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram fixedColorProgram(&fixedColorVert, nullptr, &fixedColorFrag);

    en::GLShader dirShadowVert("CGFire/dir_shadow.vert", en::GLShader::Type::VERTEX);
    en::GLShader dirShadowFrag("CGFire/dir_shadow.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram dirShadowProgram(&dirShadowVert, nullptr, &dirShadowFrag);

    en::GLShader pointShadowVert("CGFire/point_shadow.vert", en::GLShader::Type::VERTEX);
    en::GLShader pointShadowGeom("CGFire/point_shadow.geom", en::GLShader::Type::GEOMETRY);
    en::GLShader pointShadowFrag("CGFire/point_shadow.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram pointShadowProgram(&pointShadowVert, &pointShadowGeom, &pointShadowFrag);

    en::GLShader simpleVert("CGFire/simple.vert", en::GLShader::Type::VERTEX);
    en::GLShader simpleFrag("CGFire/simple.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram simpleProgram(&simpleVert, nullptr, &simpleFrag);
    simpleProgram.Use();

    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    // Models
    std::vector<glm::vec3> splinePoints = {
            { 5.0f, 10.0f, 5.0f },
            { -5.0f, 0.0f, 5.0f },
            { -5.0f, 0.0f, -5.0f },
            { 5.0f, 0.0f, -5.0f }
    };
    en::Spline3D spline(splinePoints, true, 40);
    en::Spline3DRenderable splineRenderable(&spline);
    en::RenderObj splineObj(&splineRenderable);

    en::Model backpackModel("backpack/backpack.obj", true);
    en::RenderObj backpackObj(&backpackModel);

    en::Model floorModel("cube.obj", true);
    en::RenderObj floorObj(&floorModel);
    floorObj.t_ = glm::translate(glm::vec3(0.0f, -5.0f, 0.0f)) * glm::scale(glm::vec3(50.0f, 1.0f, 50.0f));

    // Lights
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(0.5f));

    en::SimplePointLight pointLight(glm::vec3(1.0f, 1.0f, 1.0f), 200.0f);
    pointLight.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));
    std::vector<const en::PointLight*> pointLights = { (const en::PointLight*)&pointLight };

    // Main loop
    en::Log::Info("Starting main loop");
    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();
        util::HandleUserCamMovement(window, cam, deltaTime);

        // Physics
        pointLight.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight.t_;
        backpackObj.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        // Dir shadow rendering
        dirShadowProgram.Use();
        glm::mat4 dirLightMat = dirLight.GetLightMat();
        dirShadowProgram.SetUniformMat4("light_mat", false, &dirLightMat[0][0]);

        dirLight.BindShadowBuffer();
        backpackObj.Render(&dirShadowProgram);
        pointLight.Render(&dirShadowProgram);
        floorObj.Render(&dirShadowProgram);
        dirLight.UnbindShadowBuffer();

        // Point shadow rendering
        pointShadowProgram.Use();
        unsigned int pointLightCount = pointLights.size();
        if (pointLightCount > POINT_LIGHT_MAX)
        {
            en::Log::Warn("Too many point lights");
            pointLightCount = POINT_LIGHT_MAX;
        }
        for (unsigned int i = 0; i < pointLightCount; i++)
        {
            const en::PointLight* currentPointLight = pointLights[i];
            std::vector<glm::mat4> pointLightMats = currentPointLight->GetLightMats();
            for (unsigned int j = 0; j < 6; j++)
                pointShadowProgram.SetUniformMat4("point_light_mats[" + std::to_string(j) + "]", false, &pointLightMats[j][0][0]);
            pointShadowProgram.SetUniformVec3f("light_pos", currentPointLight->GetPos());
            //pointShadowProgram.SetUniformF("far_plane", 25.0f); // TODO: pointLight.GetShadowFarPlane();

            currentPointLight->BindShadowBuffer();
            backpackObj.Render(&pointShadowProgram);
            pointLight.Render(&pointShadowProgram); // Can only be done when culling the back face
            floorObj.Render(&pointShadowProgram);
            currentPointLight->UnbindShadowBuffer();
        }

        // Real rendering
        window.UseViewport();
        cam.SetAspectRatio(window.GetAspectRatio());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();

        simpleProgram.Use();
        simpleProgram.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        simpleProgram.SetUniformMat4("proj_mat", false, &projMat[0][0]);
        simpleProgram.SetUniformVec3f("cam_pos", cam.GetPos());

        dirLight.Use(&simpleProgram);
        dirLight.UseShadow(&simpleProgram);
        simpleProgram.SetUniformMat4("dir_light_mat", false, &dirLightMat[0][0]);

        simpleProgram.SetUniformUI("point_light_count", pointLightCount);
        for (unsigned int i = 0; i < pointLightCount; i++)
        {
            pointLights[i]->Use(&simpleProgram, i);
            pointLights[i]->UseShadow(&simpleProgram, i);
        }

        pointLight.Render(&simpleProgram);
        backpackObj.Render(&simpleProgram);
        floorObj.Render(&simpleProgram);

        // Test render spline
        fixedColorProgram.Use();
        fixedColorProgram.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        fixedColorProgram.SetUniformMat4("proj_mat", false, &projMat[0][0]);
        splineObj.Render(&fixedColorProgram);
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
