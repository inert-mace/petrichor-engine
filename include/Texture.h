#pragma once
#include "glad/glad.h"

struct Texture {
    GLuint id;
    int w;
    int h;
    // only used if animated is true
    int frameW;
    int frameH;
    int frameCount;
    int currFrame = 1;
    bool animated;
    Texture(int w, int h, GLuint id) : w(w), h(h), frameW(w), frameH(h), frameCount(1), animated(false), id(id) {}
    Texture(int w, int h, int frameW, int frameH, int frameCount, bool animated, GLuint id) : w(w), h(h), frameW(frameW), frameH(frameH), frameCount(frameCount), animated(animated), id(id) {}
    void advanceFrame() {
        if(currFrame >= frameCount) {
            currFrame = 1;
        }
        else {
            currFrame += 1;
        }
    }
};