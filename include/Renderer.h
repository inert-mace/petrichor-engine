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
        // Changes the ambient color
        void setAmbientColor(int r, int g, int b);
        void setAmbientIntensity(float i);
        void cacheULs();
        void addSprites();
        void loadTextures();
        void buildPipeline();
        void createBufferObjects(const GLfloat* vertices, size_t vertexBytes, const GLuint* indices, size_t indexBytes);
        GLuint VAO, VBO, EBO;
        GLuint shaderProgram = 0;
        int pixelScale = 4;
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
        GLint UL_ambientColor;
        GLint UL_ambientColorIntensity;
        GLint UL_burnColor;
        GLint UL_burnBand;
        glm::vec3 ambientColor = glm::vec3(34.0f/255.0f, 32.0f/255.0f, 52.0f/255.0f);
        glm::vec3 burnColor = glm::vec3(34.0f/255.0f, 32.0f/255.0f, 52.0f/255.0f);
        float ambientColorIntensity = 0.25f;
        float burnBand = 0.0f;
        // checks if renderer needs to sort its spriteList
        bool renderOrderDirty = false;
};