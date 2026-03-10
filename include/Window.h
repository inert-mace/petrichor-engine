#pragma once
#include "SDL3/SDL.h"

class Window
{
    public:
        Window();
        ~Window();

        // Creates the SDL window, sets attributes, and creates the OpenGL context
        int init(int width, int height);
        int getWidth() const { return _width; }
        int getHeight() const { return _height; }
        void swapBuffers() { SDL_GL_SwapWindow(_window); }
    private:
        int _width;
        int _height;
        SDL_Window* _window = nullptr;
        SDL_GLContext glContext = nullptr;
};