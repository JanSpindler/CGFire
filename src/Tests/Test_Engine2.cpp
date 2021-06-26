//
// Created by JS on 26/06/2021.
//

#include "engine/Util.hpp"
#include "engine/Window.hpp"
#include "engine/Camera.hpp"
#include "engine/render/GLShader.hpp"
#include "engine/model/Model.hpp"
#include <glm/gtx/transform.hpp>
#include "engine/render/Light.hpp"
#include "engine/prefab/SimplePointLight.hpp"
#include "engine/input/Input.hpp"
#include "engine/render/SceneRenderer.hpp"

void HandleInput(en::Window* window, en::Camera* cam, float deltaTime)
{
    // Mouse input handling
    bool mouseRightPressed = en::Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    window->EnableCursor(!mouseRightPressed);
    if (mouseRightPressed)
    {
        glm::vec2 mouseMove = 0.005f * -en::Input::GetMouseMove();
        cam->RotateViewDir(mouseMove.x, mouseMove.y);
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
    cam->Move(camMove);
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

    en::GLShader geomVert("CGFire/deferred_geometry.vert", en::GLShader::Type::VERTEX);
    en::GLShader geomFrag("CGFire/deferred_geometry.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram geomProgram(&geomVert, nullptr, &geomFrag);

    en::GLShader lightVert("CGFire/deferred_lighting.vert", en::GLShader::Type::VERTEX);
    en::GLShader lightFrag("CGFire/deferred_lighting.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram lightProgram(&lightVert, nullptr, &lightFrag);

    // Models
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

    // Scene
    en::SceneRenderer sceneRenderer(
            &geomProgram,
            &lightProgram,
            &fixedColorProgram,
            &dirShadowProgram,
            &pointShadowProgram,
            800, 600);

    sceneRenderer.SetDirLight(&dirLight);
    sceneRenderer.AddPointLight(&pointLight);
    sceneRenderer.AddStandardRenderObj(&backpackObj);
    sceneRenderer.AddStandardRenderObj(&floorObj);
    sceneRenderer.AddFixedColorRenderObj(&pointLight);

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

        HandleInput(&window, &cam, deltaTime);

        // Physics
        pointLight.t_ = glm::rotate(deltaTime * -0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * pointLight.t_;
        backpackObj.t_ *= glm::rotate(deltaTime * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        // Render
        cam.SetAspectRatio(window.GetAspectRatio());
        sceneRenderer.Render(&window, &cam);
    }

    return 0;
}
