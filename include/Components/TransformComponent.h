#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

struct TransformComponent {
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;
};