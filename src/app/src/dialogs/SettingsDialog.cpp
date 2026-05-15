#include "app/dialogs/SettingsDialog.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QScreen>
#include <QTabWidget>
#include <QVBoxLayout>

// Common resolution presets (windowed mode)
struct ResPreset {
    QString label;
    QSize size;
};

static const std::vector<ResPreset>& resolutionPresets() {
    static const std::vector<ResPreset> presets = {
        {"1280 x 720   (HD)",       QSize(1280, 720)},
        {"1366 x 768   (Laptop)",   QSize(1366, 768)},
        {"1600 x 900   (HD+)",      QSize(1600, 900)},
        {"1920 x 1080  (Full HD)",  QSize(1920, 1080)},
        {"2560 x 1440  (QHD)",      QSize(2560, 1440)},
        {"3840 x 2160  (4K)",       QSize(3840, 2160)},
        {"640 x 480    (VGA)",       QSize(640, 480)},
        {"800 x 600    (SVGA)",      QSize(800, 600)},
        {"1024 x 768   (XGA)",       QSize(1024, 768)},
    };
    return presets;
}

static int findPresetIndex(const QSize& size) {
    const auto& presets = resolutionPresets();
    for (size_t i = 0; i < presets.size(); ++i) {
        if (presets[i].size == size) return static_cast<int>(i);
    }
    return -1;
}

static QString keyToString(int key) {
    switch (key) {
    case Qt::Key_Up:      return "Up";
    case Qt::Key_Down:    return "Down";
    case Qt::Key_Left:    return "Left";
    case Qt::Key_Right:   return "Right";
    case Qt::Key_Return:  return "Enter";
    case Qt::Key_Enter:   return "Enter";
    case Qt::Key_Escape:  return "Esc";
    case Qt::Key_Space:   return "Space";
    case Qt::Key_Tab:     return "Tab";
    case Qt::Key_Shift:   return "Shift";
    case Qt::Key_Control: return "Ctrl";
    case Qt::Key_Alt:     return "Alt";
    default:              return QKeySequence(key).toString();
    }
}

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , bindings_(KeyBindings::defaults())
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(450, 380);

    auto* mainLayout = new QVBoxLayout(this);

    auto* tabs = new QTabWidget(this);
    auto* gameTab = new QWidget();
    auto* controlsTab = new QWidget();
    setupGameTab(gameTab);
    setupControlsTab(controlsTab);
    tabs->addTab(gameTab, tr("Game"));
    tabs->addTab(controlsTab, tr("Controls"));
    mainLayout->addWidget(tabs);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void SettingsDialog::updateResolutionComboState() {
    bool isWindowed = (screenModeCombo_->currentIndex() == 0);
    resolutionCombo_->setEnabled(isWindowed);
}

void SettingsDialog::setupGameTab(QWidget* tab) {
    auto* layout = new QFormLayout(tab);

    // Screen mode
    screenModeCombo_ = new QComboBox(tab);
    screenModeCombo_->addItem(tr("Windowed"));
    screenModeCombo_->addItem(tr("Fullscreen"));
    layout->addRow(tr("Screen Mode:"), screenModeCombo_);

    connect(screenModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { updateResolutionComboState(); });

    // Resolution (windowed mode only)
    resolutionCombo_ = new QComboBox(tab);
    resolutionCombo_->addItem(tr("System Default"));
    for (const auto& preset : resolutionPresets()) {
        resolutionCombo_->addItem(preset.label);
    }
    layout->addRow(tr("Resolution:"), resolutionCombo_);

    gridWidthSpin_ = new QSpinBox(tab);
    gridWidthSpin_->setRange(10, 100);
    layout->addRow(tr("Grid Width:"), gridWidthSpin_);

    gridHeightSpin_ = new QSpinBox(tab);
    gridHeightSpin_->setRange(10, 100);
    layout->addRow(tr("Grid Height:"), gridHeightSpin_);

    initialSpeedSpin_ = new QSpinBox(tab);
    initialSpeedSpin_->setRange(30, 500);
    initialSpeedSpin_->setSuffix(" ms");
    layout->addRow(tr("Initial Speed:"), initialSpeedSpin_);

    speedIncrementSpin_ = new QSpinBox(tab);
    speedIncrementSpin_->setRange(1, 20);
    speedIncrementSpin_->setSuffix(" ms");
    layout->addRow(tr("Speed Increment:"), speedIncrementSpin_);

    minSpeedSpin_ = new QSpinBox(tab);
    minSpeedSpin_->setRange(10, 200);
    minSpeedSpin_->setSuffix(" ms");
    layout->addRow(tr("Minimum Speed:"), minSpeedSpin_);

    snakeLengthSpin_ = new QSpinBox(tab);
    snakeLengthSpin_->setRange(1, 10);
    layout->addRow(tr("Initial Length:"), snakeLengthSpin_);

    // Replay directory
    auto* replayRow = new QHBoxLayout();
    replayDirEdit_ = new QLineEdit(tab);
    replayDirEdit_->setPlaceholderText(tr("Default: Documents"));
    auto* browseBtn = new QPushButton("...", tab);
    browseBtn->setFixedWidth(30);
    connect(browseBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(
            this, tr("Select Replay Directory"), replayDirEdit_->text());
        if (!dir.isEmpty()) {
            replayDirEdit_->setText(dir);
        }
    });
    replayRow->addWidget(replayDirEdit_);
    replayRow->addWidget(browseBtn);
    layout->addRow(tr("Replay Dir:"), replayRow);
}

void SettingsDialog::setupControlsTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    auto* group = new QGroupBox(tr("Key Bindings"), tab);
    auto* form = new QFormLayout(group);

    rows_ = {
        {"Up",      &bindings_.keyUp,      nullptr},
        {"Down",    &bindings_.keyDown,    nullptr},
        {"Left",    &bindings_.keyLeft,    nullptr},
        {"Right",   &bindings_.keyRight,   nullptr},
        {"Pause",   &bindings_.keyPause,   nullptr},
        {"Confirm", &bindings_.keyConfirm, nullptr},
        {"Back",    &bindings_.keyBack,    nullptr},
    };

    for (size_t i = 0; i < rows_.size(); ++i) {
        auto& row = rows_[i];
        row.button = new QPushButton(keyToString(*row.value), tab);
        row.button->setMinimumWidth(100);
        row.button->installEventFilter(this);
        int idx = static_cast<int>(i);
        connect(row.button, &QPushButton::clicked, this, [this, idx]() {
            if (capturingIndex_ == idx) {
                cancelKeyCapture();
            } else {
                if (capturingIndex_ >= 0) {
                    cancelKeyCapture();
                }
                startKeyCapture(idx);
            }
        });
        form->addRow(row.label + ":", row.button);
    }

    layout->addWidget(group);
    layout->addStretch();
}

void SettingsDialog::startKeyCapture(int index) {
    capturingIndex_ = index;
    rows_[index].button->setText("...");
    rows_[index].button->setFocus();
}

void SettingsDialog::cancelKeyCapture() {
    if (capturingIndex_ < 0) return;
    auto& row = rows_[capturingIndex_];
    row.button->setText(keyToString(*row.value));
    capturingIndex_ = -1;
}

void SettingsDialog::applyCapturedKey(int key) {
    if (capturingIndex_ < 0) return;

    int conflictIndex = -1;
    for (size_t i = 0; i < rows_.size(); ++i) {
        if (static_cast<int>(i) != capturingIndex_ && *rows_[i].value == key) {
            conflictIndex = static_cast<int>(i);
            break;
        }
    }

    if (conflictIndex >= 0) {
        std::swap(*rows_[capturingIndex_].value, *rows_[conflictIndex].value);
        rows_[conflictIndex].button->setText(keyToString(*rows_[conflictIndex].value));
    } else {
        *rows_[capturingIndex_].value = key;
    }

    rows_[capturingIndex_].button->setText(keyToString(key));
    capturingIndex_ = -1;
}

bool SettingsDialog::eventFilter(QObject* obj, QEvent* event) {
    if (capturingIndex_ >= 0 && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        int key = keyEvent->key();

        if (key == Qt::Key_Escape) {
            cancelKeyCapture();
            return true;
        }

        if (key != Qt::Key_Control && key != Qt::Key_Shift &&
            key != Qt::Key_Alt && key != Qt::Key_Meta) {
            applyCapturedKey(key);
            return true;
        }
    }

    return QDialog::eventFilter(obj, event);
}

core::GameConfig SettingsDialog::gameConfig() const {
    core::GameConfig config;
    config.gridWidth          = gridWidthSpin_->value();
    config.gridHeight         = gridHeightSpin_->value();
    config.initialSpeed       = initialSpeedSpin_->value();
    config.speedIncrement     = speedIncrementSpin_->value();
    config.minSpeed           = minSpeedSpin_->value();
    config.initialSnakeLength = snakeLengthSpin_->value();
    return config;
}

KeyBindings SettingsDialog::keyBindings() const {
    return bindings_;
}

bool SettingsDialog::fullscreen() const {
    return screenModeCombo_->currentIndex() == 1;
}

QSize SettingsDialog::windowSize() const {
    int idx = resolutionCombo_->currentIndex();
    if (idx <= 0) {
        return QSize(); // system default
    }
    size_t presetIdx = static_cast<size_t>(idx - 1);
    if (presetIdx < resolutionPresets().size()) {
        return resolutionPresets()[presetIdx].size;
    }
    return QSize();
}

void SettingsDialog::setFullscreen(bool on) {
    screenModeCombo_->setCurrentIndex(on ? 1 : 0);
    updateResolutionComboState();
}

void SettingsDialog::setWindowSize(const QSize& size) {
    if (!size.isValid()) {
        resolutionCombo_->setCurrentIndex(0);
    } else {
        int idx = findPresetIndex(size);
        resolutionCombo_->setCurrentIndex(idx >= 0 ? idx + 1 : 0);
    }
    updateResolutionComboState();
}

void SettingsDialog::setGameConfig(const core::GameConfig& config) {
    gridWidthSpin_->setValue(config.gridWidth);
    gridHeightSpin_->setValue(config.gridHeight);
    initialSpeedSpin_->setValue(config.initialSpeed);
    speedIncrementSpin_->setValue(config.speedIncrement);
    minSpeedSpin_->setValue(config.minSpeed);
    snakeLengthSpin_->setValue(config.initialSnakeLength);
}

void SettingsDialog::setKeyBindings(const KeyBindings& bindings) {
    bindings_ = bindings;
    for (auto& row : rows_) {
        row.button->setText(keyToString(*row.value));
    }
}

QString SettingsDialog::replayDir() const {
    return replayDirEdit_->text();
}

void SettingsDialog::setReplayDir(const QString& dir) {
    replayDirEdit_->setText(dir);
}
