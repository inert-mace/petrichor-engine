#pragma once
#include "glad/glad.h"

struct Texture {
    GLuint id;
    int w;
    int h;
    Texture(int w, int h, GLuint id) : w(w), h(h), id(id) {}
};