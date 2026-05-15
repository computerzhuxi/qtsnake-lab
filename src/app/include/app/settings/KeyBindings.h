#ifndef APP_SETTINGS_KEYBINDINGS_H
#define APP_SETTINGS_KEYBINDINGS_H

#include "core/engine/Direction.h"
#include <QString>
#include <map>

struct KeyBindings {
    int keyUp      = Qt::Key_W;
    int keyDown    = Qt::Key_S;
    int keyLeft    = Qt::Key_A;
    int keyRight   = Qt::Key_D;
    int keyPause   = Qt::Key_P;
    int keyConfirm = Qt::Key_Return;
    int keyBack    = Qt::Key_Escape;

    core::Direction keyToDirection(int key) const;
    int directionToKey(core::Direction d) const;

    std::map<int, QString> toMap() const;
    static KeyBindings defaults();
};

#endif // APP_SETTINGS_KEYBINDINGS_H
