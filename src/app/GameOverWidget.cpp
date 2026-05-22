#include "GameOverWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QVariant>

GameOverWidget::GameOverWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addStretch();

    m_panel = new QWidget(this);
    m_panel->setObjectName("panel");
    m_panel->setMinimumWidth(360);

    auto* panelLayout = new QVBoxLayout(m_panel);
    panelLayout->setContentsMargins(24, 20, 24, 20);
    panelLayout->setSpacing(6);

    auto* title = new QLabel(tr("游 戏 结 束"), m_panel);
    title->setObjectName("gameOverTitle");
    title->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(title);

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setSpacing(6);
    panelLayout->addLayout(m_contentLayout);

    panelLayout->addSpacing(12);

    auto* btnAgain = new QPushButton(tr("再来一局"), m_panel);
    btnAgain->setObjectName("btnPrimary");
    panelLayout->addWidget(btnAgain);

    auto* btnSave = new QPushButton(tr("保存回放"), m_panel);
    btnSave->setEnabled(false);
    panelLayout->addWidget(btnSave);

    panelLayout->addSpacing(8);

    auto* btnMenu = new QPushButton(tr("返回主菜单"), m_panel);
    btnMenu->setObjectName("btnDanger");
    panelLayout->addWidget(btnMenu);

    outerLayout->addWidget(m_panel, 0, Qt::AlignCenter);
    outerLayout->addStretch();

    connect(btnAgain, &QPushButton::clicked, this, &GameOverWidget::playAgainClicked);
    connect(btnSave, &QPushButton::clicked, this, &GameOverWidget::saveReplayClicked);
    connect(btnMenu, &QPushButton::clicked, this, &GameOverWidget::menuClicked);

    hide();
}

void GameOverWidget::setSingleResult(int score, int length, int kills, int seconds) {
    clearContent();
    buildSingleContent(score, length, kills, seconds);
}

void GameOverWidget::setMultiResults(const std::vector<GameResult>& results) {
    clearContent();
    buildMultiContent(results);
}

void GameOverWidget::clearContent() {
    while (QLayoutItem* item = m_contentLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            delete w;
        } else if (QLayout* child = item->layout()) {
            while (QLayoutItem* childItem = child->takeAt(0)) {
                if (QWidget* w2 = childItem->widget()) {
                    delete w2;
                }
                delete childItem;
            }
            delete child;
        }
        delete item;
    }
}

void GameOverWidget::buildSingleContent(int score, int length, int kills, int seconds) {
    auto* scoreLabel = new QLabel(QString::number(score), this);
    scoreLabel->setObjectName("scoreLabel");
    scoreLabel->setAlignment(Qt::AlignCenter);
    m_contentLayout->addWidget(scoreLabel);

    int min = seconds / 60;
    int sec = seconds % 60;
    auto* statsLabel = new QLabel(
        tr("长度: %1  |  击杀: %2  |  时间: %3:%4")
            .arg(length).arg(kills).arg(min).arg(sec, 2, 10, QChar('0')),
        this);
    statsLabel->setObjectName("goStats");
    statsLabel->setAlignment(Qt::AlignCenter);
    m_contentLayout->addWidget(statsLabel);
}

void GameOverWidget::buildMultiContent(const std::vector<GameResult>& results) {
    auto* subtitle = new QLabel(tr("— 最终排名 —"), this);
    subtitle->setObjectName("goSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    m_contentLayout->addWidget(subtitle);

    auto* grid = new QGridLayout;
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(2);

    QStringList headers = {"#", tr("玩家"), tr("分数"), tr("长度"), tr("击杀"), tr("状态")};
    for (int col = 0; col < 6; ++col) {
        auto* hdr = new QLabel(headers[col], this);
        hdr->setObjectName("resultHeader");
        hdr->setAlignment(Qt::AlignCenter);
        grid->addWidget(hdr, 0, col);
    }

    auto* divider = new QWidget(this);
    divider->setObjectName("resultDivider");
    divider->setFixedHeight(1);
    grid->addWidget(divider, 1, 0, 1, 6);

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        int row = static_cast<int>(i) + 2;

        auto makeCell = [&](const QString& text) -> QLabel* {
            auto* cell = new QLabel(text, this);
            cell->setObjectName("resultCell");
            cell->setAlignment(Qt::AlignCenter);
            if (r.isSelf) cell->setProperty("you", true);
            return cell;
        };

        auto* rankCell = makeCell(QString("#%1").arg(static_cast<int>(i) + 1));
        if (i == 0)
            rankCell->setProperty("rankLevel", "gold");
        else if (i == 1)
            rankCell->setProperty("rankLevel", "silver");
        else if (i == 2)
            rankCell->setProperty("rankLevel", "bronze");
        grid->addWidget(rankCell, row, 0);

        grid->addWidget(makeCell(r.name), row, 1);
        grid->addWidget(makeCell(QString::number(r.score)), row, 2);
        grid->addWidget(makeCell(QString::number(r.length)), row, 3);
        grid->addWidget(makeCell(QString::number(r.kills)), row, 4);

        auto* statusCell = makeCell(r.alive ? tr("存活") : tr("死亡"));
        statusCell->setProperty("alive", r.alive);
        grid->addWidget(statusCell, row, 5);
    }

    grid->setColumnStretch(1, 1);

    m_contentLayout->addLayout(grid);
}
