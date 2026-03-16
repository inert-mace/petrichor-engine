#include "Systems/SystemManager.h"
#include "entt/entt.hpp"
#include "Components/TransformComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/DissolveComponent.h"

void SystemManager::update(entt::registry &registry, Renderer& renderer, float deltaTime) {
    DissolveSystem(registry, deltaTime);
    RenderSystem(registry, renderer);
}

void SystemManager::updateFixed(entt::registry &registry, Renderer& renderer, float timeStep) {
    MovementSystem(registry, timeStep);
}

void SystemManager::MovementSystem(entt::registry &registry, float deltaTime) {
    auto view = registry.view<TransformComponent, VelocityComponent>();

    for(auto [entity, transformComponent, velocityComponent] : view.each()) {

        transformComponent.position.x += velocityComponent.velocity.x * deltaTime;
        transformComponent.position.y += velocityComponent.velocity.y * deltaTime;
    }
}

void SystemManager::RenderSystem(entt::registry &registry, Renderer& renderer) {
    auto view = registry.view<TransformComponent, SpriteComponent>();

    renderer.beginFrame();
    for(auto [entity, transformComponent, spriteComponent] : view.each()) {
        DissolveComponent* dissolveComponent = registry.try_get<DissolveComponent>(entity);
        renderer.submitSprite(SpriteDrawCommand{&transformComponent, &spriteComponent, dissolveComponent});
    }

    renderer.flush();
    renderer.endFrame();
}

// void SystemManager::AudioSystem(entt::registry &registry, AudioManager& audio) {}

void SystemManager::DissolveSystem(entt::registry &registry, float deltaTime) {
    auto view = registry.view<DissolveComponent>();

    for(auto [entity, dissolveComponent] : view.each()) {
        if(dissolveComponent.active) {
                dissolveComponent.dissolveProgress = (dissolveComponent.reversed) ? std::clamp(dissolveComponent.dissolveProgress + static_cast<float>(-dissolveComponent.dissolutionRate * deltaTime), 0.0f, 1.0f) : std::clamp(dissolveComponent.dissolveProgress + static_cast<float>(dissolveComponent.dissolutionRate * deltaTime), 0.0f, 1.0f);
            }
    }
}