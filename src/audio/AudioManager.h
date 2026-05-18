#ifndef SNAKE_AUDIO_AUDIOMANAGER_H
#define SNAKE_AUDIO_AUDIOMANAGER_H

/// \brief 音效类型
enum class SoundEffect { Eat, Die, MenuClick, Countdown, Go };

/// \brief 音效管理器（空壳，阶段 4 对接 QSoundEffect）
/// \details 单例，所有 play() 调用当前无效果。
///          接口先行设计，上层调用不用改写。

class AudioManager {
public:
    static AudioManager& instance();

    /// \brief 播放指定音效（当前空实现）
    void play(SoundEffect effect);

    /// \brief 设置音量（0-100，当前仅存储）
    void setVolume(int vol);

    int volume() const;

private:
    AudioManager() = default;
    int m_volume = 50;
};

#endif // SNAKE_AUDIO_AUDIOMANAGER_H
