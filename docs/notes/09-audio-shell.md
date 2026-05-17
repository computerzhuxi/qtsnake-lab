# 里程碑 9：AudioManager 音效接口

## 做什么

预留音效系统接口，阶段 4 对接 QSoundEffect 实现。当前为空壳，调用不报错不发声。

## 怎么做

- 单例 `AudioManager::instance()`
- `play(SoundEffect)` 空实现
- `setVolume(int)` / `volume()` 正常存取
- SoundEffect 枚举：Eat, Die, MenuClick, Countdown, Go

## 为什么这样做

- **接口先行**：上层 app/controller 现在就可以调用 `play(SoundEffect::Eat)`，阶段 4 只改内部实现
- **解耦**：音效加载和播放逻辑与其他模块完全隔离
