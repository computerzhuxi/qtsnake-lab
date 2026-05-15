#include "app/settings/AppSettings.h"
#include "app/settings/KeyBindings.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QStandardPaths>

AppSettings& AppSettings::instance() {
    static AppSettings s;
    return s;
}

AppSettings::AppSettings()
    : settings_("QtSnakeLab", "QtSnakeLab")
{
}

core::GameConfig AppSettings::loadGameConfig() const {
    core::GameConfig config;
    config.gridWidth  = settings_.value("game/gridWidth",  config.gridWidth).toInt();
    config.gridHeight = settings_.value("game/gridHeight", config.gridHeight).toInt();
    config.initialSpeed = settings_.value("game/initialSpeed", config.initialSpeed).toInt();
    config.speedIncrement = settings_.value("game/speedIncrement", config.speedIncrement).toInt();
    config.minSpeed = settings_.value("game/minSpeed", config.minSpeed).toInt();
    config.initialSnakeLength = settings_.value("game/initialSnakeLength", config.initialSnakeLength).toInt();
    return config;
}

void AppSettings::saveGameConfig(const core::GameConfig& config) {
    settings_.setValue("game/gridWidth",  config.gridWidth);
    settings_.setValue("game/gridHeight", config.gridHeight);
    settings_.setValue("game/initialSpeed", config.initialSpeed);
    settings_.setValue("game/speedIncrement", config.speedIncrement);
    settings_.setValue("game/minSpeed", config.minSpeed);
    settings_.setValue("game/initialSnakeLength", config.initialSnakeLength);
}

KeyBindings AppSettings::loadKeyBindings() const {
    KeyBindings bindings;
    bindings.keyUp      = settings_.value("keys/up",      bindings.keyUp).toInt();
    bindings.keyDown    = settings_.value("keys/down",    bindings.keyDown).toInt();
    bindings.keyLeft    = settings_.value("keys/left",    bindings.keyLeft).toInt();
    bindings.keyRight   = settings_.value("keys/right",   bindings.keyRight).toInt();
    bindings.keyPause   = settings_.value("keys/pause",   bindings.keyPause).toInt();
    bindings.keyConfirm = settings_.value("keys/confirm", bindings.keyConfirm).toInt();
    bindings.keyBack    = settings_.value("keys/back",    bindings.keyBack).toInt();
    return bindings;
}

void AppSettings::saveKeyBindings(const KeyBindings& bindings) {
    settings_.setValue("keys/up",      bindings.keyUp);
    settings_.setValue("keys/down",    bindings.keyDown);
    settings_.setValue("keys/left",    bindings.keyLeft);
    settings_.setValue("keys/right",   bindings.keyRight);
    settings_.setValue("keys/pause",   bindings.keyPause);
    settings_.setValue("keys/confirm", bindings.keyConfirm);
    settings_.setValue("keys/back",    bindings.keyBack);
}

bool AppSettings::loadIntegerScaling() const {
    return settings_.value("display/integerScaling", true).toBool();
}

void AppSettings::saveIntegerScaling(bool on) {
    settings_.setValue("display/integerScaling", on);
}

QSize AppSettings::loadWindowSize() const {
    QSize def = systemDefaultSize();
    int w = settings_.value("display/windowWidth",  def.width()).toInt();
    int h = settings_.value("display/windowHeight", def.height()).toInt();
    return QSize(w, h);
}

void AppSettings::saveWindowSize(const QSize& size) {
    settings_.setValue("display/windowWidth",  size.width());
    settings_.setValue("display/windowHeight", size.height());
}

QSize AppSettings::systemDefaultSize() {
    // Use ~70% of the primary screen's available geometry
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect avail = screen->availableGeometry();
        int w = avail.width()  * 7 / 10;
        int h = avail.height() * 7 / 10;
        return QSize(w, h);
    }
    return QSize(800, 600);
}

bool AppSettings::loadFullscreen() const {
    return settings_.value("display/fullscreen", false).toBool();
}

void AppSettings::saveFullscreen(bool on) {
    settings_.setValue("display/fullscreen", on);
}

QString AppSettings::loadReplayDir() const {
    return settings_.value("paths/replayDir",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    ).toString();
}

void AppSettings::saveReplayDir(const QString& dir) {
    settings_.setValue("paths/replayDir", dir);
}
