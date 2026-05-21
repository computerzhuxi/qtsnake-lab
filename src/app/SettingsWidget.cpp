#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QStackedWidget>
#include <QStyle>

SettingsWidget::SettingsWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addStretch();

    auto* panel = new QWidget(this);
    panel->setObjectName("settingsPanel");
    panel->setMinimumWidth(360);
    panel->setMaximumWidth(480);

    outerLayout->addWidget(panel, 0, Qt::AlignCenter);
    outerLayout->addStretch();

    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(32, 20, 32, 20);
    panelLayout->setSpacing(0);

    auto* title = new QLabel(tr("设 置"), panel);
    title->setObjectName("settingsTitle");
    title->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(title);
    panelLayout->addSpacing(16);

    // --- Custom tab buttons ---
    auto* tabLayout = new QHBoxLayout;
    tabLayout->setSpacing(0);

    m_btnControl = new QPushButton(tr("操作"), panel);
    m_btnControl->setObjectName("settingsTab");
    m_btnControl->setProperty("active", true);
    m_btnControl->setFlat(true);

    m_btnGame = new QPushButton(tr("游戏"), panel);
    m_btnGame->setObjectName("settingsTab");
    m_btnGame->setProperty("active", false);
    m_btnGame->setFlat(true);

    tabLayout->addWidget(m_btnControl);
    tabLayout->addWidget(m_btnGame);
    panelLayout->addLayout(tabLayout);
    panelLayout->addSpacing(16);

    // --- QStackedWidget ---
    m_stack = new QStackedWidget(panel);

    // Page 0: 操作
    auto* controlPage = new QWidget(panel);
    auto* controlLayout = new QVBoxLayout(controlPage);
    controlLayout->setSpacing(8);

    auto* keyLabel = new QLabel(tr("移动键位"), controlPage);
    keyLabel->setObjectName("settingsLabel");

    auto* keyCombo = new QComboBox(controlPage);
    keyCombo->setObjectName("settingsCombo");
    keyCombo->addItem(tr("方向键"));
    keyCombo->addItem("WASD");

    auto* langLabel = new QLabel(tr("语言 / Language"), controlPage);
    langLabel->setObjectName("settingsLabel");

    auto* langCombo = new QComboBox(controlPage);
    langCombo->setObjectName("settingsCombo");
    langCombo->addItem(tr("中文"));
    langCombo->addItem("English");

    controlLayout->addWidget(keyLabel);
    controlLayout->addWidget(keyCombo);
    controlLayout->addWidget(langLabel);
    controlLayout->addWidget(langCombo);
    controlLayout->addStretch();

    // Page 1: 游戏
    auto* gamePage = new QWidget(panel);
    auto* gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setSpacing(8);

    auto* speedLabel = new QLabel(tr("游戏速度"), gamePage);
    speedLabel->setObjectName("settingsLabel");

    auto* speedCombo = new QComboBox(gamePage);
    speedCombo->setObjectName("settingsCombo");
    speedCombo->addItems({tr("慢 (150ms)"), tr("中 (100ms)"), tr("快 (60ms)")});
    speedCombo->setCurrentIndex(1);

    auto* sizeLabel = new QLabel(tr("棋盘大小"), gamePage);
    sizeLabel->setObjectName("settingsLabel");

    auto* sizeCombo = new QComboBox(gamePage);
    sizeCombo->setObjectName("settingsCombo");
    sizeCombo->addItems({tr("小 (15×15)"), tr("中 (20×20)"), tr("大 (30×30)")});
    sizeCombo->setCurrentIndex(1);

    auto* volLabel = new QLabel(tr("音量"), gamePage);
    volLabel->setObjectName("settingsLabel");

    auto* volRow = new QHBoxLayout;
    auto* volSlider = new QSlider(Qt::Horizontal, gamePage);
    volSlider->setObjectName("settingsSlider");
    volSlider->setRange(0, 100);
    volSlider->setValue(50);

    auto* volValue = new QLabel("50", gamePage);
    volValue->setObjectName("settingsSliderValue");

    connect(volSlider, &QSlider::valueChanged, this, [volValue](int v) {
        volValue->setText(QString::number(v));
    });

    volRow->addWidget(volSlider, 1);
    volRow->addWidget(volValue);

    gameLayout->addWidget(speedLabel);
    gameLayout->addWidget(speedCombo);
    gameLayout->addWidget(sizeLabel);
    gameLayout->addWidget(sizeCombo);
    gameLayout->addWidget(volLabel);
    gameLayout->addLayout(volRow);
    gameLayout->addStretch();

    m_stack->addWidget(controlPage);
    m_stack->addWidget(gamePage);
    panelLayout->addWidget(m_stack);

    panelLayout->addSpacing(16);

    auto* btnBack = new QPushButton(tr("返回"), panel);
    connect(btnBack, &QPushButton::clicked, this, &SettingsWidget::backClicked);
    panelLayout->addWidget(btnBack);

    // --- Tab switch ---
    connect(m_btnControl, &QPushButton::clicked, this, [this]() {
        m_stack->setCurrentIndex(0);
        m_btnControl->setProperty("active", true);
        m_btnGame->setProperty("active", false);
        refreshStyle(m_btnControl);
        refreshStyle(m_btnGame);
    });

    connect(m_btnGame, &QPushButton::clicked, this, [this]() {
        m_stack->setCurrentIndex(1);
        m_btnControl->setProperty("active", false);
        m_btnGame->setProperty("active", true);
        refreshStyle(m_btnControl);
        refreshStyle(m_btnGame);
    });

    hide();
}

void SettingsWidget::refreshStyle(QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
}
