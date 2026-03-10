#pragma once
#include "Texture.h"

struct Sprite {
    // note: will swap to using either a texture manager or some sort of smart pointer system
    // to manage texture loading and unloading more efficiently
    Texture* texture;
    float x;
    float y;
};