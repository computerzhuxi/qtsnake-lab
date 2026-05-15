#ifndef APP_SETTINGS_APPSETTINGS_H
#define APP_SETTINGS_APPSETTINGS_H

#include <QSettings>
#include <QSize>

#include "core/engine/GameConfig.h"

struct KeyBindings;

class AppSettings {
public:
    static AppSettings& instance();

    core::GameConfig loadGameConfig() const;
    void saveGameConfig(const core::GameConfig& config);

    KeyBindings loadKeyBindings() const;
    void saveKeyBindings(const KeyBindings& bindings);

    bool loadIntegerScaling() const;
    void saveIntegerScaling(bool on);

    QSize loadWindowSize() const;
    void saveWindowSize(const QSize& size);

    bool loadFullscreen() const;
    void saveFullscreen(bool on);

    QString loadReplayDir() const;
    void saveReplayDir(const QString& dir);

    static QSize systemDefaultSize();

private:
    AppSettings();
    QSettings settings_;
};

#endif // APP_SETTINGS_APPSETTINGS_H
