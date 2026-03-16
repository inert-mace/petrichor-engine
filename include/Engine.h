#pragma once
#include "Window.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "glm/glm.hpp"
#include "entt/entt.hpp"
#include "Systems/SystemManager.h"
#include <memory>

class Engine
{
public:
    Engine();
    ~Engine();

    int init();
    void run();
    // fixed-timestep simulation
    void simulate(double deltaTime);
    glm::vec2 inputDirection();
    void animation(double deltaTime, double& elapsedTime, double timePerFrame, Texture& texture);

    private:
        Window window;
        SystemManager systems;
        Renderer renderer;
        AudioManager audioManager;
        std::unique_ptr<entt::registry> registry;
        const double fixedTimeStep = 0.01667;
        const double maxAccumulatedTime = 0.25;
        double accumulator = 0.0;
};