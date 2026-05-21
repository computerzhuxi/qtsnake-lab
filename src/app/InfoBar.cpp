#include "InfoBar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>

InfoBar::InfoBar(QWidget* parent) : QWidget(parent) {
    setObjectName("infoBar");
    setFixedHeight(48);

    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(16, 0, 16, 0);
    row->setSpacing(20);
    row->setAlignment(Qt::AlignVCenter);

    row->addStretch();

    auto addStat = [&](const QString& labelText, const QString& valueObjName,
                       const QString& initialValue) -> QLabel* {
        auto* col = new QVBoxLayout;
        col->setSpacing(0);
        col->setAlignment(Qt::AlignCenter);

        auto* label = new QLabel(labelText, this);
        label->setObjectName("hudLabel");

        auto* value = new QLabel(initialValue, this);
        value->setObjectName(valueObjName);

        col->addWidget(label);
        col->addWidget(value);

        row->addLayout(col);
        return value;
    };

    auto addSep = [&]() -> QWidget* {
        auto* sep = new QWidget(this);
        sep->setObjectName("hudDivider");
        sep->setFixedSize(1, 28);
        row->addWidget(sep);
        return sep;
    };

    // Score column
    m_scoreValue = addStat("Score", "scoreValue", "0");

    addSep();

    // Length column
    m_lengthValue = addStat("Length", "lengthValue", "3");

    addSep();

    // Time column
    m_timeValue = addStat("Time", "timeValue", "00:00");

    addSep();

    // Speed column
    m_speedValue = addStat("Speed", "speedValue", "MID");

    // Separator before Kills
    m_sepBeforeKills = addSep();

    // Kills column
    m_killsLabel = new QLabel("Kills", this);
    m_killsLabel->setObjectName("hudLabel");
    m_killsValue = new QLabel("0", this);
    m_killsValue->setObjectName("killsValue");
    {
        auto* col = new QVBoxLayout;
        col->setSpacing(0);
        col->setAlignment(Qt::AlignCenter);
        col->addWidget(m_killsLabel);
        col->addWidget(m_killsValue);
        row->addLayout(col);
    }

    // Separator before Rank
    m_sepBeforeRank = addSep();

    // Rank column
    m_rankLabel = new QLabel("Rank", this);
    m_rankLabel->setObjectName("hudLabel");
    m_rankValue = new QLabel("1 / 1", this);
    m_rankValue->setObjectName("rankValue");
    {
        auto* col = new QVBoxLayout;
        col->setSpacing(0);
        col->setAlignment(Qt::AlignCenter);
        col->addWidget(m_rankLabel);
        col->addWidget(m_rankValue);
        row->addLayout(col);
    }

    row->addStretch();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setMode(false);
}

void InfoBar::setScore(int score) {
    m_scoreValue->setText(QString::number(score));
}

void InfoBar::setLength(int length) {
    m_lengthValue->setText(QString::number(length));
}

void InfoBar::setTime(int seconds) {
    int min = seconds / 60;
    int sec = seconds % 60;
    m_timeValue->setText(QString("%1:%2")
        .arg(min, 2, 10, QChar('0'))
        .arg(sec, 2, 10, QChar('0')));
}

void InfoBar::setSpeed(int speedMs) {
    QString label;
    if (speedMs < 80)
        label = "FAST";
    else if (speedMs <= 120)
        label = "MID";
    else
        label = "SLOW";
    m_speedValue->setText(label);
}

void InfoBar::setKills(int kills) {
    m_killsValue->setText(QString::number(kills));
}

void InfoBar::setRank(int rank, int total) {
    m_rankValue->setText(QString("%1 / %2").arg(rank).arg(total));
}

void InfoBar::setMode(bool multiplayer) {
    m_sepBeforeKills->setVisible(multiplayer);
    m_killsLabel->setVisible(multiplayer);
    m_killsValue->setVisible(multiplayer);

    m_sepBeforeRank->setVisible(multiplayer);
    m_rankLabel->setVisible(multiplayer);
    m_rankValue->setVisible(multiplayer);
}
