#include <iostream>
#include "Window.h"
#include "SDL3/SDL.h"

Window::Window()
{
    std::cout << "Window constructor called" << std::endl;
}

Window::~Window()
{
    std::cout << "Window destructor called" << std::endl;
}

int Window::init(int width, int height)
{
    std::cout << "Window init called" << std::endl;

    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    window = SDL_CreateWindow("Petrichor Engine", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(window == nullptr)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return -1;
    }

    return 0;
}