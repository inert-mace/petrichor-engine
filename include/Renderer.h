#pragma once
#include "SDL3/SDL.h"
#include "glad/glad.h"

class Renderer
{
    public:
        Renderer();
        ~Renderer();
        int init();
};