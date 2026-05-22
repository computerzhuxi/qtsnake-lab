#include "PauseWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

PauseWidget::PauseWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addStretch();

    auto* panel = new QWidget(this);
    panel->setObjectName("panel");
    panel->setMinimumWidth(300);
    panel->setMaximumWidth(400);

    auto* innerLayout = new QVBoxLayout(panel);
    innerLayout->setContentsMargins(24, 20, 24, 20);
    innerLayout->setSpacing(6);

    auto* title = new QLabel(tr("游戏暂停"), panel);
    title->setObjectName("pauseTitle");
    title->setAlignment(Qt::AlignCenter);
    innerLayout->addWidget(title);
    innerLayout->addSpacing(20);

    auto* btnResume = new QPushButton(tr("继续游戏"), panel);
    btnResume->setObjectName("btnPrimary");
    innerLayout->addWidget(btnResume);

    auto* btnRestart = new QPushButton(tr("重新开始"), panel);
    innerLayout->addWidget(btnRestart);

    auto* btnSettings = new QPushButton(tr("设置"), panel);
    innerLayout->addWidget(btnSettings);

    innerLayout->addSpacing(8);

    auto* btnMenu = new QPushButton(tr("返回主菜单"), panel);
    btnMenu->setObjectName("btnDanger");
    innerLayout->addWidget(btnMenu);

    outerLayout->addWidget(panel, 0, Qt::AlignCenter);
    outerLayout->addStretch();

    connect(btnResume, &QPushButton::clicked, this, &PauseWidget::resumeClicked);
    connect(btnRestart, &QPushButton::clicked, this, &PauseWidget::restartClicked);
    connect(btnSettings, &QPushButton::clicked, this, &PauseWidget::settingsClicked);
    connect(btnMenu, &QPushButton::clicked, this, &PauseWidget::menuClicked);

    hide();
}
