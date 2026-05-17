#include "PauseWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

PauseWidget::PauseWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("游戏暂停"), this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* btnResume = new QPushButton(tr("继续游戏"), this);
    btnResume->setObjectName("btnPrimary");

    auto* btnRestart = new QPushButton(tr("重新开始"), this);
    auto* btnSettings = new QPushButton(tr("设置"), this);

    auto* btnMenu = new QPushButton(tr("返回主菜单"), this);
    btnMenu->setObjectName("btnDanger");

    connect(btnResume, &QPushButton::clicked, this, &PauseWidget::resumeClicked);
    connect(btnRestart, &QPushButton::clicked, this, &PauseWidget::restartClicked);
    connect(btnSettings, &QPushButton::clicked, this, &PauseWidget::settingsClicked);
    connect(btnMenu, &QPushButton::clicked, this, &PauseWidget::menuClicked);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(btnResume, 0, Qt::AlignCenter);
    layout->addWidget(btnRestart, 0, Qt::AlignCenter);
    layout->addWidget(btnSettings, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(btnMenu, 0, Qt::AlignCenter);
    layout->addStretch();

    hide();
}
