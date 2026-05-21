#include "GamePage.h"
#include "InfoBar.h"
#include "LeaderboardWidget.h"
#include "PauseWidget.h"
#include "GameOverWidget.h"
#include "ShineLabel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QStyle>

GamePage::GamePage(QWidget* parent) : QWidget(parent) {
    m_scene = new GameScene(this);
    m_view = new GameView(this);
    m_view->setScene(m_scene);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_infoBar = new InfoBar(this);
    root->addWidget(m_infoBar);

    auto* body = new QHBoxLayout;
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(0);

    body->addWidget(m_view, 1);

    m_leaderboard = new LeaderboardWidget(this);
    body->addWidget(m_leaderboard);

    root->addLayout(body, 1);

    m_pauseWidget = new PauseWidget(this);
    m_gameOver = new GameOverWidget(this);

    m_countdownLabel = new QLabel(this);
    m_countdownLabel->setObjectName("CountdownLabel");
    m_countdownLabel->setAlignment(Qt::AlignCenter);
    m_countdownLabel->hide();

    m_shineLabel = new ShineLabel(this);
    m_shineLabel->setObjectName("shineLabel");
    m_shineLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    m_shineLabel->setFont(QFont("Consolas", 18));
    m_shineLabel->hide();

    connect(m_pauseWidget, &PauseWidget::resumeClicked, this, &GamePage::resumeClicked);
    connect(m_pauseWidget, &PauseWidget::restartClicked, this, &GamePage::restartClicked);
    connect(m_pauseWidget, &PauseWidget::settingsClicked, this, &GamePage::settingsClicked);
    connect(m_pauseWidget, &PauseWidget::menuClicked, this, &GamePage::menuClicked);
    connect(m_gameOver, &GameOverWidget::playAgainClicked, this, &GamePage::playAgainClicked);
    connect(m_gameOver, &GameOverWidget::menuClicked, this, &GamePage::menuClicked);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GameScene* GamePage::scene() const { return m_scene; }
GameView* GamePage::view() const { return m_view; }

void GamePage::enter() {
    m_scene->clearAll();
}

void GamePage::exit() {
    m_scene->clearAll();
    hideAllOverlays();
}

void GamePage::showPause() {
    hideAllOverlays();
    m_pauseWidget->show();
    m_pauseWidget->raise();
}

void GamePage::showGameOver(int score, int length, int kills, int seconds) {
    hideAllOverlays();
    m_gameOver->setSingleResult(score, length, kills, seconds);
    m_gameOver->show();
    m_gameOver->raise();
}

void GamePage::showCountdown(int number) {
    if (number == -1) {
        m_countdownLabel->hide();
        m_shineLabel->setText(tr("按任意键开始"));
        m_shineLabel->show();
        m_shineLabel->raise();
        m_shineLabel->startShine();
        return;
    }

    m_shineLabel->stopShine();
    m_shineLabel->hide();

    QString text;
    Qt::Alignment align;

    if (number == 0) {
        text = "GO!";
        m_countdownLabel->setProperty("state", "go");
        align = Qt::AlignCenter;
    } else {
        text = QString::number(number);
        m_countdownLabel->setProperty("state", "count");
        align = Qt::AlignCenter;
    }

    m_countdownLabel->style()->unpolish(m_countdownLabel);
    m_countdownLabel->style()->polish(m_countdownLabel);
    m_countdownLabel->setAlignment(align);
    m_countdownLabel->setText(text);
    m_countdownLabel->show();
    m_countdownLabel->raise();
}

void GamePage::hideAllOverlays() {
    m_pauseWidget->hide();
    m_gameOver->hide();
    m_countdownLabel->hide();
    m_shineLabel->stopShine();
    m_shineLabel->hide();
}

void GamePage::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    int w = width();
    int h = height();
    m_pauseWidget->setGeometry(0, 0, w, h);
    m_gameOver->setGeometry(0, 0, w, h);
    m_countdownLabel->setGeometry(0, 0, w, h);
    m_shineLabel->setGeometry(0, 0, w, static_cast<int>(h * 0.95));

    if (m_scene && !m_scene->sceneRect().isEmpty()) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}
