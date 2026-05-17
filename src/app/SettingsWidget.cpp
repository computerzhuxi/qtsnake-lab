#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QComboBox>
#include <QSlider>

SettingsWidget::SettingsWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("设置"), this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* tabWidget = new QTabWidget(this);

    auto* controlTab = new QWidget(this);
    auto* controlLayout = new QVBoxLayout(controlTab);

    auto* keyLabel = new QLabel(tr("移动键位:"), controlTab);
    auto* keyCombo = new QComboBox(controlTab);
    keyCombo->addItem(tr("方向键"));
    keyCombo->addItem("WASD");

    auto* langLabel = new QLabel(tr("语言 / Language:"), controlTab);
    auto* langCombo = new QComboBox(controlTab);
    langCombo->addItem(tr("中文"));
    langCombo->addItem("English");

    controlLayout->addWidget(keyLabel);
    controlLayout->addWidget(keyCombo);
    controlLayout->addWidget(langLabel);
    controlLayout->addWidget(langCombo);
    controlLayout->addStretch();

    auto* gameTab = new QWidget(this);
    auto* gameLayout = new QVBoxLayout(gameTab);

    auto* speedLabel = new QLabel(tr("游戏速度:"), gameTab);
    auto* speedCombo = new QComboBox(gameTab);
    speedCombo->addItems({tr("慢"), tr("中"), tr("快")});
    speedCombo->setCurrentIndex(1);

    auto* sizeLabel = new QLabel(tr("棋盘大小:"), gameTab);
    auto* sizeCombo = new QComboBox(gameTab);
    sizeCombo->addItems({tr("小 (15x15)"), tr("中 (20x20)"), tr("大 (30x30)")});
    sizeCombo->setCurrentIndex(1);

    auto* volLabel = new QLabel(tr("音量:"), gameTab);
    auto* volSlider = new QSlider(Qt::Horizontal, gameTab);
    volSlider->setRange(0, 100);
    volSlider->setValue(50);

    gameLayout->addWidget(speedLabel);
    gameLayout->addWidget(speedCombo);
    gameLayout->addWidget(sizeLabel);
    gameLayout->addWidget(sizeCombo);
    gameLayout->addWidget(volLabel);
    gameLayout->addWidget(volSlider);
    gameLayout->addStretch();

    tabWidget->addTab(controlTab, tr("操作"));
    tabWidget->addTab(gameTab, tr("游戏"));

    auto* btnBack = new QPushButton(tr("返回"), this);
    connect(btnBack, &QPushButton::clicked, this, &SettingsWidget::backClicked);

    mainLayout->addWidget(title);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(tabWidget, 0, Qt::AlignCenter);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(btnBack, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    hide();
}
