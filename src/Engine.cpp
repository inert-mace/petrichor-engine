#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include <iostream>
#include <algorithm>
#include "SDL3/SDL.h"
#include "glm/geometric.hpp"
#include "Components/TransformComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/DissolveComponent.h"
#include "Systems/SystemManager.h"

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
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        throw std::runtime_error(std::string("SDL initialization failed:") + SDL_GetError());
    }

    // initialize window
    window.init(1280, 720);

    audioManager.init();
    audioManager.createSound("../assets/battle_theme.wav", "battle_theme");
    audioManager.playSound("battle_theme", true);

    // initialize renderer
    renderer.init(window);

    // initialize registry
    registry = std::make_unique<entt::registry>();
    auto testEntity = registry->create();
    registry->emplace<TransformComponent>(testEntity, glm::vec3(640.0f, 360.0f, 1.0f), glm::vec4(0.0f), glm::vec3(1.0f));
    registry->emplace<SpriteComponent>(testEntity, SpriteComponent("title", BACKGROUND));
    registry->emplace<DissolveComponent>(testEntity, DissolveComponent{"column_effect", true, true, 1.0f, 0.6f});

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
        systems.updateFixed(*registry, renderer, fixedTimeStep);

        accumulator -= fixedTimeStep;
    }
    
}

void Engine::run()
{
    // counter ticks; ticks/second is stored in the SDL_GetPerformanceFrequency
    uint64_t lastFrame = SDL_GetPerformanceCounter();
    double deltaTime;
    double elapsedAnimationTime = 0.0;
    const double inverseFrequency = 1.0 / SDL_GetPerformanceFrequency();
    bool shouldQuit = false;
    bool reversed = true;
    float dissolutionRate = 0.5f;
    while(true)
    {
        uint64_t currentFrame = SDL_GetPerformanceCounter();
        // calculate deltaTime, using inverse frequency because multiplication is a faster operation than division
        deltaTime = (currentFrame - lastFrame) * inverseFrequency;
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
        audioManager.update();
        //animation(deltaTime, elapsedAnimationTime, 0.1, renderer.textures.at("slash"));

        // renderer.render(); rendering is called by RenderSystem now
        systems.update(*registry, renderer, deltaTime);

        window.swapBuffers();

        lastFrame = currentFrame;
    }
}

void Engine::animation(double deltaTime, double& elapsedTime, double timePerFrame, Texture& texture) {
    elapsedTime += deltaTime;
    if(elapsedTime >= timePerFrame) {
        texture.advanceFrame();
        elapsedTime -= timePerFrame;
    }
}