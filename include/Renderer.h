#pragma once
#include "Window.h"
#include "Texture.h"
#include "TextureRegion.h"
#include "SDL3/SDL.h"
#include "glad/glad.h"

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        // Initializes the renderer and uses GLAD to load OpenGL function pointers
        int init(Window& window);

        // Renders the current frame
        void render();
        GLuint VAO, VBO, EBO, shaderProgram, texture;
};