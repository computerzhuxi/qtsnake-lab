#include "RenderComponent.h"
#include "GameScene.h"

RenderComponent::RenderComponent(GameScene* scene) : m_scene(scene) {}

void RenderComponent::update(GameState& state) {
    m_scene->syncFromState(state);
}
