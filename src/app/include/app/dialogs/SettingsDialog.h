#ifndef APP_DIALOGS_SETTINGSDIALOG_H
#define APP_DIALOGS_SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QSize>

#include "core/engine/GameConfig.h"
#include "app/settings/KeyBindings.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    core::GameConfig gameConfig() const;
    KeyBindings keyBindings() const;
    QSize windowSize() const;
    bool fullscreen() const;
    QString replayDir() const;

    void setGameConfig(const core::GameConfig& config);
    void setKeyBindings(const KeyBindings& bindings);
    void setWindowSize(const QSize& size);
    void setFullscreen(bool on);
    void setReplayDir(const QString& dir);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupGameTab(QWidget* tab);
    void setupControlsTab(QWidget* tab);
    void startKeyCapture(int index);
    void cancelKeyCapture();
    void applyCapturedKey(int key);
    void updateResolutionComboState();

    // Display widgets
    QComboBox* screenModeCombo_ = nullptr;
    QComboBox* resolutionCombo_ = nullptr;

    // Game config widgets
    QLineEdit* replayDirEdit_ = nullptr;
    QSpinBox* gridWidthSpin_ = nullptr;
    QSpinBox* gridHeightSpin_ = nullptr;
    QSpinBox* initialSpeedSpin_ = nullptr;
    QSpinBox* speedIncrementSpin_ = nullptr;
    QSpinBox* minSpeedSpin_ = nullptr;
    QSpinBox* snakeLengthSpin_ = nullptr;

    // Key binding state
    KeyBindings bindings_;
    struct KeyBindingRow {
        QString label;
        int* value;
        QPushButton* button;
    };
    std::vector<KeyBindingRow> rows_;
    int capturingIndex_ = -1;
};

#endif // APP_DIALOGS_SETTINGSDIALOG_H
