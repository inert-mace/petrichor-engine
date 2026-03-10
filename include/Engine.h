#pragma once
#include "Window.h"
#include "Renderer.h"

class Engine
{
public:
    Engine();
    ~Engine();

    int init();
    void run();

    private:
        Window window;
        Renderer renderer;
};