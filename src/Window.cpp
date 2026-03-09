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

int Window::init()
{
    std::cout << "Window init called" << std::endl;

    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("Petrichor Engine", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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