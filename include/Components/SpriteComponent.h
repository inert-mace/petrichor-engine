#pragma once
#include <string>

enum LAYER {
    BACKGROUND, // 0
    WORLD, // 1
    EFFECTS // 2
};
struct SpriteComponent {
    // TODO: create a texture manager for texture load/get
    std::string textureKey;

    int layer;
    SpriteComponent(std::string textureKey, int layer) : textureKey(textureKey), layer(layer) {}
};