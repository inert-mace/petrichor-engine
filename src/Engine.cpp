#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include <iostream>
#include "SDL3/SDL.h"


int Engine::init()
{
    // initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO ) < 0)
    {
        throw std::runtime_error(std::string("SDL initialization failed:") + SDL_GetError());
    }

    // initialize window
    Window *window = new Window();
    window->init();

    // initialize renderer
    Renderer *renderer = new Renderer();
    renderer->init();

    return 0;
}

void Engine::run()
{
    // main loop
}

void Engine::clean()
{
    // clean up resources
    /*
    if(_renderer) { 
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }

    if(_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }*/
    // quit SDL
    SDL_Quit();
}