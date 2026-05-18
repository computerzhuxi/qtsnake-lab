#include "GameOverWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

GameOverWidget::GameOverWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("游戏结束"), this);
    title->setObjectName("gameOverTitle");
    title->setAlignment(Qt::AlignCenter);

    auto* scoreLabel = new QLabel(this);
    scoreLabel->setObjectName("scoreLabel");
    scoreLabel->setAlignment(Qt::AlignCenter);

    auto* statsLabel = new QLabel(this);
    statsLabel->setObjectName("subtitleLabel");
    statsLabel->setAlignment(Qt::AlignCenter);

    auto* btnAgain = new QPushButton(tr("再来一局"), this);
    btnAgain->setObjectName("btnPrimary");

    auto* btnSave = new QPushButton(tr("保存回放"), this);
    btnSave->setEnabled(false);

    auto* btnMenu = new QPushButton(tr("返回主菜单"), this);
    btnMenu->setObjectName("btnDanger");

    connect(btnAgain, &QPushButton::clicked, this, &GameOverWidget::playAgainClicked);
    connect(btnSave, &QPushButton::clicked, this, &GameOverWidget::saveReplayClicked);
    connect(btnMenu, &QPushButton::clicked, this, &GameOverWidget::menuClicked);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(8);
    layout->addWidget(scoreLabel);
    layout->addWidget(statsLabel);
    layout->addSpacing(16);
    layout->addWidget(btnAgain, 0, Qt::AlignCenter);
    layout->addWidget(btnSave, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(btnMenu, 0, Qt::AlignCenter);
    layout->addStretch();

    hide();
}

void GameOverWidget::setScore(int score, int length, int kills, int seconds) {
    auto* s = findChild<QLabel*>("scoreLabel");
    if (s) s->setText(QString::number(score));

    int min = seconds / 60;
    int sec = seconds % 60;
    auto* st = findChild<QLabel*>("subtitleLabel");
    if (st) st->setText(
        tr("长度: %1  |  击杀: %2  |  时间: %3:%4")
            .arg(length).arg(kills).arg(min).arg(sec, 2, 10, QChar('0')));
}
