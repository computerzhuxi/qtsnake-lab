#ifndef SNAKE_CONTROLLER_RENDERCOMPONENT_H
#define SNAKE_CONTROLLER_RENDERCOMPONENT_H

#include "GameState.h"

class GameScene;

/// \brief 渲染组件：GameState → GameScene::syncFromState()
class RenderComponent {
public:
    explicit RenderComponent(GameScene* scene);
    void update(GameState& state);

private:
    GameScene* m_scene;  ///< 关联的渲染场景（不负责生命周期）
};

#endif
