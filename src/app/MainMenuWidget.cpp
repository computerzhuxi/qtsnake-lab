#include "MainMenuWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainMenuWidget::MainMenuWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);

    auto* panel = new QWidget(this);
    panel->setObjectName("panel");
    panel->setFixedWidth(400);

    auto* layout = new QVBoxLayout(panel);
    layout->setSpacing(0);

    // 装饰蛇 + 分割线
    auto* topLine = new QWidget(panel);
    topLine->setObjectName("decorLine");
    topLine->setFixedHeight(1);

    auto* snakeDecor = new QWidget(panel);
    auto* snakeLayout = new QHBoxLayout(snakeDecor);
    snakeLayout->setAlignment(Qt::AlignCenter);
    snakeLayout->setSpacing(4);
    for (int i = 0; i < 6; ++i) {
        auto* seg = new QWidget(snakeDecor);
        seg->setObjectName(i == 0 ? "snakeHead" : "snakeBody");
        seg->setFixedSize(8, 8);
        snakeLayout->addWidget(seg);
    }

    auto* bottomLine = new QWidget(panel);
    bottomLine->setObjectName("decorLine");
    bottomLine->setFixedHeight(1);

    // 标题
    auto* title = new QLabel("SNAKE ARENA", panel);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel(tr("MULTI-MODE BATTLE PLATFORM"), panel);
    subtitle->setObjectName("subtitleLabel");
    subtitle->setAlignment(Qt::AlignCenter);

    // SELECT MODE hint
    auto* hint = new QLabel(tr("▼ SELECT MODE"), panel);
    hint->setObjectName("hintLabel");
    hint->setAlignment(Qt::AlignCenter);

    // 按钮
    auto* btnSingle = new QPushButton(tr("单人经典模式"), panel);
    auto* btnAI = new QPushButton(tr("AI 对战模式"), panel);
    auto* btnMulti = new QPushButton(tr("联机对战模式"), panel);
    auto* btnReplay = new QPushButton(tr("对局回放"), panel);

    btnSingle->setObjectName("btnPrimary");
    btnSingle->setCursor(Qt::PointingHandCursor);
    btnAI->setEnabled(false);
    btnMulti->setEnabled(false);
    btnReplay->setEnabled(false);

    connect(btnSingle, &QPushButton::clicked, this, &MainMenuWidget::singlePlayerClicked);
    connect(btnAI, &QPushButton::clicked, this, &MainMenuWidget::aiBattleClicked);
    connect(btnMulti, &QPushButton::clicked, this, &MainMenuWidget::multiplayerClicked);
    connect(btnReplay, &QPushButton::clicked, this, &MainMenuWidget::replayClicked);

    // 底部
    auto* bottomLayout = new QHBoxLayout();
    auto* btnSettings = new QPushButton(tr("[ 设置 ]"), panel);
    btnSettings->setObjectName("btnSettings");
    auto* btnExit = new QPushButton(tr("[ 退出 ]"), panel);
    btnExit->setObjectName("btnExit");
    btnSettings->setCursor(Qt::PointingHandCursor);
    btnExit->setCursor(Qt::PointingHandCursor);
    connect(btnSettings, &QPushButton::clicked, this, &MainMenuWidget::settingsClicked);
    connect(btnExit, &QPushButton::clicked, this, &MainMenuWidget::exitClicked);

    bottomLayout->addStretch();
    bottomLayout->addWidget(btnSettings);
    bottomLayout->addWidget(btnExit);
    bottomLayout->addStretch();

    layout->addWidget(topLine);
    layout->addWidget(snakeDecor);
    layout->addWidget(bottomLine);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(hint, 0, Qt::AlignCenter);
    layout->addSpacing(16);
    layout->addWidget(btnSingle);
    layout->addSpacing(6);
    layout->addWidget(btnAI);
    layout->addSpacing(6);
    layout->addWidget(btnMulti);
    layout->addSpacing(6);
    layout->addWidget(btnReplay);
    layout->addSpacing(20);
    layout->addLayout(bottomLayout);

    outerLayout->addWidget(panel);

    hide();
}
