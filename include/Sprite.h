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
    bool dissolve;
    float maskPanY = 1.0f;
    float dissolveProgress = 0.0f;
    Sprite(float x, float y, float z, float scaleX, float scaleY, float scaleZ, std::string textureKey, bool dissolve) : x(x), y(y), z(z), scaleX(scaleX), scaleY(scaleY), scaleZ(scaleZ), textureKey(textureKey), dissolve(dissolve) {}
};