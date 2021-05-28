#include "engine/Util.hpp"
#include "engine/Window.hpp"
#include "engine/Camera.hpp"

int main(int argc, char** argv)
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");

    float fov = glm::radians(60.f);
    en::Camera cam(
            glm::vec3(0.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            (float) window.GetWidth() / (float) window.GetHeight(),
            fov);

    while (window.IsOpen())
    {
        window.Update();
        cam.SetAspectRatio((float) window.GetWidth() / (float) window.GetHeight());
    }

    return 0;
}
