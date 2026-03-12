#pragma once
#include <string>

struct Sprite {
    // note: will swap to using either a texture manager or some sort of smart pointer system
    // to manage texture loading and unloading more efficiently
    std::string textureKey;
    float x;
    float y;
    float z;
    float scaleX;
    float scaleY;
    float scaleZ;
    Sprite(float x, float y, float z, float scaleX, float scaleY, float scaleZ, std::string textureKey) : x(x), y(y), z(z), scaleX(scaleX), scaleY(scaleY), scaleZ(scaleZ), textureKey(textureKey) {}
};