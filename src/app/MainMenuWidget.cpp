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
    topLine->setFixedHeight(1);
    topLine->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 transparent,stop:0.5 #00ff8866,stop:1 transparent);");

    auto* snakeDecor = new QWidget(panel);
    auto* snakeLayout = new QHBoxLayout(snakeDecor);
    snakeLayout->setAlignment(Qt::AlignCenter);
    snakeLayout->setSpacing(4);
    for (int i = 0; i < 6; ++i) {
        auto* seg = new QWidget(snakeDecor);
        seg->setFixedSize(8, 8);
        seg->setStyleSheet(QString("background: %1; border-radius: 2px;")
            .arg(i == 0 ? "#00ff88" : "#00bb55"));
        snakeLayout->addWidget(seg);
    }

    auto* bottomLine = new QWidget(panel);
    bottomLine->setFixedHeight(1);
    bottomLine->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 transparent,stop:0.5 #00ff8866,stop:1 transparent);");

    // 标题
    auto* title = new QLabel("SNAKE ARENA", panel);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-family: monospace; font-size: 28px; letter-spacing: 8px; color: #e0e0ff; padding-top: 12px;");

    auto* subtitle = new QLabel(tr("MULTI-MODE BATTLE PLATFORM"), panel);
    subtitle->setObjectName("subtitleLabel");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-family: monospace; font-size: 9px; letter-spacing: 4px; color: #556; padding-bottom: 16px;");

    // SELECT MODE hint
    auto* hint = new QLabel(tr("▼ SELECT MODE"), panel);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color: #0ff; font-size: 10px; border: 1px solid #1a1a3e; border-radius: 10px; padding: 3px 12px; font-family: monospace;");

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
    auto* btnExit = new QPushButton(tr("[ 退出 ]"), panel);
    btnSettings->setStyleSheet("QPushButton { background: transparent; border: none; color: #556; font-size: 11px; min-width: 0; padding: 4px 12px; font-family: monospace; } QPushButton:hover { color: #00ff88; }");
    btnExit->setStyleSheet("QPushButton { background: transparent; border: none; color: #556; font-size: 11px; min-width: 0; padding: 4px 12px; font-family: monospace; } QPushButton:hover { color: #ff6688; }");
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
