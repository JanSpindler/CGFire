#include "engine/Util.hpp"
#include "engine/Window.hpp"

int main(int argc, char** argv)
{
    en::Log::Info("Initializing CGFire");

    en::Window window(800, 600, "CGFire");
    while (window.IsOpen())
    {
        window.Update();
    }

    return 0;
}
