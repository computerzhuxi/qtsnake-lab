#include "app/dialogs/LeaderboardDialog.h"
#include "app/leaderboard/LeaderboardData.h"
#include <QDateTime>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

LeaderboardDialog::LeaderboardDialog(const LeaderboardData& data,
                                     QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Leaderboard"));
    setMinimumSize(450, 350);

    auto* layout = new QVBoxLayout(this);

    table_ = new QTableWidget(this);
    table_->setColumnCount(5);
    table_->setHorizontalHeaderLabels({
        tr("Rank"), tr("Name"), tr("Score"),
        tr("Grid"), tr("Time")
    });
    table_->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto entries = data.topN(10);
    table_->setRowCount(static_cast<int>(entries.size()));

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        auto* rankItem = new QTableWidgetItem(
            QString::number(static_cast<int>(i + 1)));
        auto* nameItem = new QTableWidgetItem(
            QString::fromStdString(e.playerName));
        auto* scoreItem = new QTableWidgetItem(
            QString::number(e.score));
        auto* gridItem = new QTableWidgetItem(
            QString("%1x%2").arg(e.gridWidth).arg(e.gridHeight));
        auto* timeItem = new QTableWidgetItem(
            QDateTime::fromSecsSinceEpoch(e.timestamp)
                .toString("yyyy-MM-dd hh:mm"));

        if (i == 0) {
            QFont bold = rankItem->font();
            bold.setBold(true);
            rankItem->setFont(bold);
            nameItem->setFont(bold);
            scoreItem->setFont(bold);
        }

        rankItem->setTextAlignment(Qt::AlignCenter);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        gridItem->setTextAlignment(Qt::AlignCenter);

        table_->setItem(static_cast<int>(i), 0, rankItem);
        table_->setItem(static_cast<int>(i), 1, nameItem);
        table_->setItem(static_cast<int>(i), 2, scoreItem);
        table_->setItem(static_cast<int>(i), 3, gridItem);
        table_->setItem(static_cast<int>(i), 4, timeItem);
    }

    layout->addWidget(table_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
