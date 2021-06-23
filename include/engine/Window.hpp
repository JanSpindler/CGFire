//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_WINDOW_HPP
#define CGFIRE_WINDOW_HPP


struct GLFWwindow;

namespace en
{
    // Die Window Klasse kapselt GLFW und OpenGL Funktionalitäten.
    class Window
    {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        void Update();
        void Destroy();

        void EnableCursor(bool cursorMode);
        void UseViewport() const;


        GLFWwindow* GetHandle(){ return handle_; }
        int GetWidth() const;
        int GetHeight() const;
        float GetAspectRatio() const;
        bool IsOpen();

    private:
        GLFWwindow* handle_;
        int width_;
        int height_;
        bool cursorEnabled_;
    };
}

#endif //CGFIRE_WINDOW_HPP
