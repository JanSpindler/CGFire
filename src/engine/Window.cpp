//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/Window.hpp"
#include "engine/Util.hpp"

namespace en
{
    void resize_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Window::Window(int width, int height, const char* title)
    {
        handle_ = nullptr;

        // Init GLFW
        Log::Info("Initializing GLFW");
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create Window
        Log::Info("Creating GLFW Window");
        handle_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (handle_ == nullptr)
        {
            Log::Error("Failed to create GLFW Window", true);
            Destroy();
            return;
        }
        glfwSetWindowSizeCallback(handle_, resize_callback);
        glfwMakeContextCurrent(handle_);

        // OpenGL Setup
        gladLoadGL();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Window::~Window()
    {
        if (handle_ != nullptr)
            Destroy();
    }

    void Window::Update()
    {
        // Image has just been drawn

        // Check for Errors after drawing
        while(PopGLError(false) != GL_NO_ERROR);

        // Clear image and check for Window updates
        glfwSwapBuffers(handle_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        // Clear Error before drawing
        while (!glGetError() != GL_NO_ERROR);

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

    bool Window::IsOpen()
    {
        return !glfwWindowShouldClose(handle_);
    }
}
