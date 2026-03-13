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
        // loads a texture (overload: static)
        void loadTexture(const std::string& filePath, const std::string& key, GLint horizontalWrapMode, GLint verticalWrapMode);
        // loads a texture (overload: animated)
        void loadTexture(const std::string& filePath, const std::string& key, GLint horizontalWrapMode, GLint verticalWrapMode, int frameX, int frameY, int frameCount, bool animated);
        // Renders the current frame
        void render();
        GLuint VAO, VBO, EBO, shaderProgram;
        int pixelScale = 8;
        glm::mat4 uProjection, uModel;
        std::unordered_map<std::string, Texture> textures;

        // temp location; this will go into engine later
        // but I want to test model transforms/positions.
        std::vector<Sprite> spriteList;
        GLint UL_uModel;
        GLint UL_uTexture;
        GLint UL_animated;
        GLint UL_frame;
        GLint UL_dissolve;
        GLint UL_dissolveProgress;
        GLint UL_dissolveMaskSize;
        GLint UL_spriteTextureSize;
        GLint UL_maskOffset;
        int dissolveHalfWidth;
};