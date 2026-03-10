#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include <iostream>
#include "SDL3/SDL.h"

Engine::Engine()
{
    std::cout << "Engine constructor called" << std::endl;
    init();
}

Engine::~Engine()
{
    std::cout << "Engine destructor called" << std::endl;
}

int Engine::init()
{
    // initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO ) < 0)
    {
        throw std::runtime_error(std::string("SDL initialization failed:") + SDL_GetError());
    }

    // initialize window
    window.init(1280, 720);

    // initialize renderer
    renderer.init(window);

    return 0;
}

void Engine::run()
{
    bool shouldQuit = false;
    while(true)
    {
        // handle events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            {
                std::cout << "Escape key pressed, quitting..." << std::endl;
                shouldQuit = true;
                break;
            }
        }
        if(shouldQuit) break;
        // render
        renderer.render();
        window.swapBuffers();
    }
}