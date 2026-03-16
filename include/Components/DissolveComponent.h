#pragma once
#include <string>

struct DissolveComponent {
    // current types: dissolve_effect, slash_effect, slash_effect_fast, claw_effect, columns
    std::string dissolveType = "dissolve_effect";
    bool active;
    bool reversed;
    float dissolveProgress = 0.0f;
    // NOTE: dissolution rate is in absolute value
    float dissolutionRate = 0.2f;
};