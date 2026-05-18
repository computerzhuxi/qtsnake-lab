# 里程碑 13：输入源策略模式

## 做什么

将输入从 Controller 解耦为策略接口 IInputProvider，后续 AI/网络/回放只需实现新策略，不用改 Controller。

## 接口

```cpp
class IInputProvider {
public:
    virtual ~IInputProvider() = default;
    virtual Direction getDirection(int snakeIdx, const GameState& state) = 0;
};
```

## 具体实现

- `KeyboardInput`：AppShell 传入 QKeyEvent，内部存方向，getDirection() 返回当前方向
- `AIInput`（阶段 2）：BFS 寻路返回方向
- `NetworkInput`（阶段 3）：网络包解析方向
- `ReplayInput`（阶段 2）：回放文件读方向

## Controller 变化

- 移除 `handleKeyPress(Direction)` ——方向不再由外部直接设
- tick() 开头调用 m_input->getDirection() 注入方向到蛇
- 状态机、计时、渲染逻辑不变

## AppShell 变化

- 创建 KeyboardInput，传给 Controller
- keyPressEvent 中方向键 → KeyboardInput::setDirection(Direction)
- ESC / 回车仍走 Controller（暂停、倒计时等非输入操作）

## 为什么这样做

- Controller 不对具体输入源编码，开闭原则
- 阶段 2-3 加 AI/网络/回放时不需改 Controller 代码
- IInputProvider 无 Qt 依赖，可以独立单元测试
