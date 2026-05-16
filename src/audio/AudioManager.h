#pragma once

enum class SoundEffect { Eat, Die, MenuClick, Countdown, Go };

/// \brief 音效管理器（当前为空壳接口，阶段4对接 QSoundEffect）
class AudioManager {
public:
    static AudioManager& instance();
    void play(SoundEffect effect);
    void setVolume(int vol);
    int volume() const;

private:
    AudioManager() = default;
    int m_volume = 50;
};
