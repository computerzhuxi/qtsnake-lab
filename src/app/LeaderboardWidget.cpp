#include "LeaderboardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVariant>

LeaderboardWidget::LeaderboardWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("leaderboard");
    setFixedWidth(170);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    auto* title = new QLabel(QString::fromUtf8("\xe2\x80\x94 RANK \xe2\x80\x94"), this);
    title->setObjectName("rankTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addStretch();

    m_aliveLabel = new QLabel("Alive: 0 / 0", this);
    m_aliveLabel->setObjectName("rankAlive");
    m_aliveLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_aliveLabel);
}

void LeaderboardWidget::updatePlayers(const std::vector<PlayerInfo>& players) {
    clearRows();

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(this->layout());

    for (size_t i = 0; i < players.size(); ++i) {
        QWidget* row = makePlayerRow(static_cast<int>(i) + 1, players[i]);
        m_rows.append(row);
        layout->insertWidget(static_cast<int>(i) + 1, row);
    }

    int aliveCount = 0;
    for (const auto& p : players) {
        if (p.alive) ++aliveCount;
    }
    m_aliveLabel->setText(
        QString("Alive: %1 / %2").arg(aliveCount).arg(static_cast<int>(players.size())));
}

QWidget* LeaderboardWidget::makePlayerRow(int rank, const PlayerInfo& info) {
    auto* row = new QWidget(this);
    row->setObjectName("rankRow");
    row->setProperty("self", info.isSelf);
    row->setProperty("alive", info.alive);

    auto* hbox = new QHBoxLayout(row);
    hbox->setContentsMargins(6, 2, 6, 2);
    hbox->setSpacing(6);

    auto* rankNum = new QLabel(QString::number(rank), row);
    rankNum->setObjectName("rankNum");
    if (rank == 1)
        rankNum->setProperty("rankLevel", "gold");
    else if (rank == 2)
        rankNum->setProperty("rankLevel", "silver");
    else if (rank == 3)
        rankNum->setProperty("rankLevel", "bronze");

    auto* dot = new QWidget(row);
    dot->setObjectName("rankDot");
    dot->setFixedSize(8, 8);
    dot->setProperty("dotAlive", info.alive);

    auto* name = new QLabel(info.name, row);
    name->setObjectName("rankName");
    name->setProperty("alive", info.alive);

    auto* score = new QLabel(QString::number(info.score), row);
    score->setObjectName("rankScore");
    score->setProperty("alive", info.alive);

    hbox->addWidget(rankNum);
    hbox->addWidget(dot);
    hbox->addWidget(name, 1);
    hbox->addWidget(score);

    return row;
}

void LeaderboardWidget::clearRows() {
    for (QWidget* w : m_rows) {
        static_cast<QVBoxLayout*>(layout())->removeWidget(w);
        delete w;
    }
    m_rows.clear();
}
