#ifndef SNAKE_CONTROLLER_RENDERCOMPONENT_H
#define SNAKE_CONTROLLER_RENDERCOMPONENT_H

#include "GameState.h"

class GameScene;

/// \brief 渲染组件：GameState → GameScene::syncFromState()
/// \details 调用时机：每 tick 起始，渲染上一帧结束态；碰撞帧不再次渲染，
///          玩家停留在撞墙前合法位置（这是有意为之的 UX，非缺陷）。
class RenderComponent {
public:
    explicit RenderComponent(GameScene* scene);
    void update(GameState& state);

private:
    GameScene* m_scene;  ///< 关联的渲染场景（不负责生命周期）
};

#endif
