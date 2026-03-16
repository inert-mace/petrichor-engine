#pragma once
#include <unordered_map>
#include <string>
#include "Texture.h"

class TextureManager {
    public:
    // Attempts to retrieve a texture from its texture map.
        void getTexture();
    private:
        std::unordered_map<std::string, Texture> textures;
};