#pragma once
#include "Window.h"
#include "Texture.h"
#include "Sprite.h"
#include "TextureRegion.h"
#include "SDL3/SDL.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        // Initializes the renderer and uses GLAD to load OpenGL function pointers
        int init(Window& window);

        // Renders the current frame
        void render();
        GLuint VAO, VBO, EBO, shaderProgram;
        int pixelScale = 8;
        glm::mat4 uProjection, uModel;
        std::unordered_map<std::string, Texture> textures;

        // temp location; this will go into engine later
        // but I want to test model transforms/positions.
        std::vector<Sprite> spriteList;
};