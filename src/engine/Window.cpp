//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/Input/Input.hpp"

namespace en
{
    void ErrorCallback(int error, const char* desc)
    {
        std::string errorStr = "GLFW Error " + std::to_string(error) + " - " + desc;
        Log::Error(errorStr.c_str(), false);
    }

    Window::Window(int width, int height, const char* title)
    {
        handle_ = nullptr;
        width_ = width;
        height_ = height;
        cursorEnabled_ = true;

        // Init GLFW
        Log::Info("Initializing GLFW");
        if (!glfwInit())
        {
            Log::Error("Failed to initialize GLFW", true);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create Window
        Log::Info("Creating GLFW Window");
        handle_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (handle_ == nullptr)
        {
            Log::Error("Failed to create GLFW Window", true);
        }
        glfwSetErrorCallback(ErrorCallback);
        glfwMakeContextCurrent(handle_);
        glfwSwapInterval(0);
        Input::Init(handle_);

        // OpenGL Setup
        gladLoadGL();
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Log::Error("Failed to load GLAD", true);
        }
        ClearGLError();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        PopGLError(true);
        Log::Info("OpenGL and GLFW have been initialized");
    }

    Window::~Window()
    {
        if (handle_ != nullptr)
            Destroy();
    }

    void Window::Update()
    {
        // Image has just been drawn
        glfwSwapBuffers(handle_);

        // Check for Errors after drawing
        while(PopGLError(true) != GL_NO_ERROR);

        glfwPollEvents();
        glfwGetWindowSize(handle_, &width_, &height_);

        // Image will be drawn after this function
    }

    void Window::Destroy()
    {
        Log::Info("Destroying GLFW Window");
        glfwDestroyWindow(handle_);
        handle_ = nullptr;

        Log::Info("Terminating GLFW");
        glfwTerminate();
    }

    void Window::EnableCursor(bool cursorMode)
    {
        if (cursorMode)
        {
            if (!cursorEnabled_)
            {
                glfwSetInputMode(handle_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursorEnabled_ = true;
            }
        }
        else
        {
            if (cursorEnabled_)
            {
                glfwSetInputMode(handle_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorEnabled_ = false;
            }
        }
    }

    void Window::UseViewport() const
    {
        glViewport(0, 0, width_, height_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    int Window::GetWidth() const
    {
        return width_;
    }

    int Window::GetHeight() const
    {
        return height_;
    }

    float Window::GetAspectRatio() const
    {
        float aspectRatio = 1.0f;
        if (height_ != 0)
            aspectRatio = (float) width_ / (float) height_;
        return aspectRatio;
    }

    bool Window::IsOpen()
    {
        return !glfwWindowShouldClose(handle_);
    }
}
