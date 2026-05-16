# 里程碑 6：audio 音效接口

## 做什么

预留音效系统接口，阶段 4 对接 QSoundEffect 实现。当前为空壳，调用不报错也不发声。

## 怎么做

- AudioManager 单例，play(SoundEffect) 空实现，setVolume/getVolume 正常存取
- SoundEffect 枚举定义类型：Eat, Die, MenuClick, Countdown, Go

## 为什么这样做

- **接口先行**：上层 app/controller 可以现在就调用 `AudioManager::instance().play(SoundEffect::Eat)`，阶段 4 只需改 AudioManager 内部实现，调用方不动
- **解耦**：音效加载和播放逻辑与其他模块完全隔离，各自独立开发和测试
