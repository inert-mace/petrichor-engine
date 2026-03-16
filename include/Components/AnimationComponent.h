#pragma once

#include "glm/vec2.hpp"

struct AnimationComponent {
    glm::vec2 frameDimensions;
    int currentFrame = 0;
};