#ifndef APP_DIALOGS_LEADERBOARDDIALOG_H
#define APP_DIALOGS_LEADERBOARDDIALOG_H

#include <QDialog>
#include <QTableWidget>

class LeaderboardData;

class LeaderboardDialog : public QDialog {
    Q_OBJECT

public:
    explicit LeaderboardDialog(const LeaderboardData& data,
                               QWidget* parent = nullptr);

private:
    QTableWidget* table_;
};

#endif // APP_DIALOGS_LEADERBOARDDIALOG_H
