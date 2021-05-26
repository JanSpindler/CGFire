#include "engine/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include "engine/GLShader.hpp"
#include "engine/Model.hpp"
#include "engine/Renderer.hpp"

//#include "engine/config.hpp"
//#include "engine/gr_include.hpp"

struct Vert
{
    glm::vec3 pos;
};

int main()
{
    en::Log::Info("Initializing CGFire");

    en::Time::Update();
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

    glm::mat4 modelMat = glm::scale(glm::identity<glm::mat4>(), glm::vec3(5.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 10.0f));
    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    en::Model model("backpack/backpack.obj", true);

    en::DirectionalLight dirLight;
    dirLight.dir_ = glm::normalize(glm::vec3(-1.0, -0.4f, 1.0f));
    dirLight.color_ = glm::vec3(1.0f);

    en::Renderer renderer(&program);
    renderer.SetDirectionalLight(dirLight);

    while (window.IsOpen())
    {
        window.Update();

        cam.SetAspectRatio(window.GetAspectRatio());
        viewMat = cam.GetViewMat();
        projMat = cam.GetProjMat();

        modelMat = glm::rotate(modelMat, (float)(en::Time::GetDeltaTime() * 1.0), glm::vec3(0.0f, 1.0f, 0.0f));

        program.Use();
        program.SetUniformMat4("model_mat", false, &modelMat[0][0]);
        program.SetUniformMat4("view_mat", false, &viewMat[0][0]);
        program.SetUniformMat4("proj_mat", false, &projMat[0][0]);

        renderer.Render(&model);
    }

    en::Log::Info("Ending CGFire");
    return 0;
}
