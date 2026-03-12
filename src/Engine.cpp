#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include <iostream>
#include "SDL3/SDL.h"
#include "glm/geometric.hpp"

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

glm::vec2 Engine::inputDirection() {
    const bool *key_states = SDL_GetKeyboardState(NULL);
    glm::vec2 direction = glm::vec2(0.0f);

    if (key_states[SDL_SCANCODE_W]) {
        direction.y += -1;
    } 
    if (key_states[SDL_SCANCODE_A]) {
        direction.x += -1;
    }
    if (key_states[SDL_SCANCODE_S]) {
        direction.y += 1;
    } 
    if (key_states[SDL_SCANCODE_D]) {
        direction.x += 1;
    }

    // normalize non-zero length direction vectors to account for diagonals
    if(direction.x != 0 || direction.y != 0) direction = glm::normalize(direction);

    return direction;
}

// for physics simulations and other things that should run on fixed, framerate independent time steps
void Engine::simulate(double deltaTime) {
    float testSpeed = 200.00f;
    accumulator += deltaTime;
    while(accumulator >= fixedTimeStep)
    {
        glm::vec2 direction = inputDirection();

        renderer.spriteList[0].x += testSpeed * direction.x * deltaTime;
        renderer.spriteList[0].y += testSpeed * direction.y * deltaTime;

        accumulator -= fixedTimeStep;
    }
    
}

void Engine::run()
{
    // counter ticks; ticks/second is stored in the SDL_GetPerformanceFrequency
    uint64_t lastFrame = SDL_GetPerformanceCounter();
    double deltaTime;
    const double inverseFrequency = 1.0 / SDL_GetPerformanceFrequency();
    bool shouldQuit = false;
    while(true)
    {
        uint64_t currentFrame = SDL_GetPerformanceCounter();
        // calculate deltaTime, using inverse frequency because multiplication is a faster operation than division
        deltaTime = (currentFrame - lastFrame) * inverseFrequency;
        std::cout << deltaTime << std::endl;
        // handle events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_KEY_DOWN) {
                if(event.key.key == SDLK_ESCAPE)
                {
                    std::cout << "Escape key pressed, quitting..." << std::endl;
                    shouldQuit = true;
                    break;
                }
            }
        }
        simulate(deltaTime);

        if(shouldQuit) break;
        // render
        renderer.render();
        window.swapBuffers();

        // std::cout << "Sprite at (" << renderer.spriteList[0].x << " x, " << renderer.spriteList[0].y << " y)" << std::endl;
        lastFrame = currentFrame;
    }
}