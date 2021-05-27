#include "engine/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/GLShader.hpp"
#include "engine/Model.hpp"
#include "engine/Light.hpp"
#include "engine/Input.hpp"

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

    float fov = glm::radians(60.f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    en::Camera cam(
            glm::vec3(0.0f, 3.0f, -10.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            window.GetAspectRatio(),
            fov,
            nearPlane,
            farPlane);

    en::GLShader vertShader("simple.vert", en::GLShader::Type::VERTEX);
    en::GLShader fragShader("simple.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram program(vertShader, fragShader);

    glm::mat4 modelMat = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 10.0f));
    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    en::Model model("backpack/backpack.obj", true);

    program.Use();
    en::DirLight dirLight(glm::vec3(0.3f, -0.4, 1.0f), glm::vec3(1.0f));
    dirLight.Use(&program);

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
        float camMoveSpeed = 2.0f * deltaTime;
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

        // Rendering
        cam.SetAspectRatio(window.GetAspectRatio());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();

        modelMat = glm::rotate(modelMat, deltaTime * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));

        program.Use();
        program.SetUniformMat4("model_mat", false, &modelMat[0][0]);
        program.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        program.SetUniformMat4("proj_mat", false, &projMat[0][0]);
        model.Draw(&program);
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
