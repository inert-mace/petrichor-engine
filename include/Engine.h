#pragma once
#include "Window.h"
#include "Renderer.h"
#include "glm/glm.hpp"

class Engine
{
public:
    Engine();
    ~Engine();

    int init();
    void run();
    glm::vec2 inputDirection();


    private:
        Window window;
        Renderer renderer;
};