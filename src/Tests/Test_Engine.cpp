#include "engine/Window.hpp"

#include <glm/gtx/transform.hpp>

#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/Render/GLShader.hpp"
#include "engine/Model/Model.hpp"
#include "engine/Render/Light.hpp"
#include "engine/Input/Input.hpp"
#include "engine/prefab/SimplePointLight.hpp"

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

    float fov = glm::radians(60.f);
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -20.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            fov,
            nearPlane,
            farPlane);

    // Simple shader
    en::GLShader simpleVert("CGFire/simple.vert", en::GLShader::Type::VERTEX);
    en::GLShader simpleFrag("CGFire/simple.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram simpleProgram(simpleVert, simpleFrag);
    simpleProgram.Use();

    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    // Models
    en::Model backpackModel("backpack/backpack.obj", true);
    en::RenderObj backpackObj = { &backpackModel };

    en::Model floorModel("cube.obj", true);
    en::RenderObj floorObj = { &floorModel };
    floorObj.t_ = glm::translate(glm::vec3(0.0f, -5.0f, 0.0f)) * glm::scale(glm::vec3(25.0f, 1.0f, 25.0f));

    // Lights
    en::DirLight dirLight(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(0.5f));

    en::SimplePointLight pointLight(glm::vec3(1.0f, 1.0f, 1.0f), 200.0f);
    pointLight.t_ = glm::translate(glm::vec3(0.0f, 10.0f, 15.0f));
    std::vector<const en::PointLight*> pointLights = { (const en::PointLight*)&pointLight };
    en::PointLightBatch plBatch(pointLights);

    en::GLShader shadowVert("CGFire/shadow.vert", en::GLShader::Type::VERTEX);
    en::GLShader shadowFrag("CGFire/shadow.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram shadowProgram(shadowVert, shadowFrag);

    // Main loop
    while (window.IsOpen())
    {
        window.Update();
        en::Input::Update();
        en::Time::Update();
        float deltaTime = (float)en::Time::GetDeltaTime();

        // Mouse input handling
        bool mouseRightPressed = en::Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        window.EnableCursor(!mouseRightPressed);
        if (mouseRightPressed)
        {
            glm::vec2 mouseMove = 0.005f * -en::Input::GetMouseMove();
            cam.RotateViewDir(mouseMove.x, mouseMove.y);
        }

        // Keyboard input handling
        glm::vec3 camMove(0.0f, 0.0f, 0.0f);
        float camMoveSpeed = 20.0f * deltaTime;
        bool frontPressed = en::Input::IsKeyPressed(KEY_W);
        bool backPressed = en::Input::IsKeyPressed(KEY_S);
        bool leftPressed = en::Input::IsKeyPressed(KEY_A);
        bool rightPressed = en::Input::IsKeyPressed(KEY_D);
        bool upPressed = en::Input::IsKeyPressed(KEY_SPACE);
        bool downPressed = en::Input::IsKeyPressed(KEY_C);
        if (frontPressed && !backPressed)
            camMove.z = camMoveSpeed;
        else if (backPressed && !frontPressed)
            camMove.z = -camMoveSpeed;
        if (rightPressed && !leftPressed)
            camMove.x = camMoveSpeed;
        else if (leftPressed  && !rightPressed)
            camMove.x = -camMoveSpeed;
        if (upPressed && !downPressed)
            camMove.y = camMoveSpeed;
        else if (downPressed && !upPressed)
            camMove.y = -camMoveSpeed;
        cam.Move(camMove);

        // Physics
        pointLight.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight.t_;
        backpackObj.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        // Shadow rendering
        shadowProgram.Use();
        glm::mat4 dirLightMat = dirLight.GetLightMat();
        shadowProgram.SetUniformMat4("light_mat", false, &dirLightMat[0][0]);

        dirLight.BindShadowBuffer();

        backpackObj.Render(&shadowProgram);
        floorObj.Render(&shadowProgram);

        dirLight.UnbindShadowBuffer();

        // Real rendering
        window.UseViewport();
        cam.SetAspectRatio(window.GetAspectRatio());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();

        simpleProgram.Use();
        simpleProgram.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        simpleProgram.SetUniformMat4("proj_mat", false, &projMat[0][0]);
        simpleProgram.SetUniformVec3f("cam_pos", cam.GetPos());
        simpleProgram.SetUniformMat4("light_mat", false, &dirLightMat[0][0]);
        dirLight.UseShadow(&simpleProgram);

        dirLight.Use(&simpleProgram);
        plBatch.Use(&simpleProgram);

        pointLight.Render(&simpleProgram);
        backpackObj.Render(&simpleProgram);
        floorObj.Render(&simpleProgram);
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
