#include "GamePage.h"
#include "PauseWidget.h"
#include "GameOverWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>

GamePage::GamePage(QWidget* parent) : QWidget(parent) {
    m_scene = new GameScene(this);
    m_view = new GameView(this);
    m_view->setScene(m_scene);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    m_pauseWidget = new PauseWidget(this);
    m_gameOver = new GameOverWidget(this);

    m_countdownLabel = new QLabel(this);
    m_countdownLabel->setObjectName("CountdownLabel");
    m_countdownLabel->setAlignment(Qt::AlignCenter);
    m_countdownLabel->hide();

    connect(m_pauseWidget, &PauseWidget::resumeClicked, this, &GamePage::resumeClicked);
    connect(m_pauseWidget, &PauseWidget::restartClicked, this, &GamePage::restartClicked);
    connect(m_pauseWidget, &PauseWidget::settingsClicked, this, &GamePage::settingsClicked);
    connect(m_pauseWidget, &PauseWidget::menuClicked, this, &GamePage::menuClicked);
    connect(m_gameOver, &GameOverWidget::playAgainClicked, this, &GamePage::playAgainClicked);
    connect(m_gameOver, &GameOverWidget::menuClicked, this, &GamePage::menuClicked);
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
    m_gameOver->setScore(score, length, kills, seconds);
    m_gameOver->show();
    m_gameOver->raise();
}

void GamePage::showCountdown(int number) {
    QString text;
    Qt::Alignment align;

    if (number == -1) {
        text = tr("按任意键开始");
        m_countdownLabel->setProperty("state", "hint");
        align = Qt::AlignHCenter | Qt::AlignBottom;
    } else if (number == 0) {
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
}

void GamePage::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    int w = width();
    int h = height();
    m_pauseWidget->setGeometry(0, 0, w, h);
    m_gameOver->setGeometry(0, 0, w, h);
    m_countdownLabel->setGeometry(0, 0, w, h);

    if (m_scene && !m_scene->sceneRect().isEmpty()) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}
