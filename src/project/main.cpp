#include "engine/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/GLShader.hpp"
#include "engine/Mesh.hpp"
#include "engine/AssetManager.hpp"

struct Vert
{
    glm::vec3 pos;
};

int main(int argc, char** argv)
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

    float fov = glm::radians(60.f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    en::Camera cam(
            glm::vec3(0.0f, 0.0f, -10.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            (float) window.GetWidth() / (float) window.GetHeight(),
            fov,
            nearPlane,
            farPlane);

    en::GLShader vertShader("simple.vert", en::GLShader::Type::VERTEX);
    en::GLShader fragShader("simple.frag", en::GLShader::Type::FRAGMENT);
    en::GLProgram program(vertShader, fragShader);

    en::Mesh mesh = en::AssetManager::LoadMesh("dragon.obj");

    glm::mat4 modelMat = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 10.0f));
    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::vec4 color(1.0f, 0.0f, 1.0f, 1.0f);

    program.Use();
    program.SetUniformVec4f("color", color);

    while (window.IsOpen())
    {
        window.Update();

        cam.SetAspectRatio((float) window.GetWidth() / (float) window.GetHeight());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();

        program.Use();
        program.SetUniformMat4("model_mat", false, &modelMat[0][0]);
        program.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        program.SetUniformMat4("proj_mat", false, &projMat[0][0]);

        mesh.Draw();
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
