#pragma once
#include "entt/entt.hpp"
#include "Renderer.h"
#include "AudioManager.h"

class SystemManager {
    public:
        // For systems that should go into the main update loop.
        void update(entt::registry &registry, Renderer& renderer, float deltaTime);
        // For systems that need fixed timesteps to function properly.
        void updateFixed(entt::registry &registry, Renderer& renderer, float timeStep);
        // Requires a Transform and Velocity. Applies velocity to transform.
        void MovementSystem(entt::registry &registry, float deltaTime);
        // Requires a Transform and Sprite. Renders a sprite to screen.
        void RenderSystem(entt::registry &registry, Renderer& renderer);
        // TODO: create an audio component and tie audio instances to the ECS
        //void AudioSystem(entt::registry &registry, AudioManager& audio);

        // Requires a Dissolve. Manages state of dissolution effect.
        void DissolveSystem(entt:: registry &registry, float deltaTime);
};