#ifndef APP_DIALOGS_MULTIPLAYERDIALOG_H
#define APP_DIALOGS_MULTIPLAYERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>

class MultiplayerDialog : public QDialog {
    Q_OBJECT

public:
    explicit MultiplayerDialog(QWidget* parent = nullptr);

    bool isHosting() const;
    QString hostAddress() const;
    quint16 port() const;
    QString playerName() const;

private:
    QLineEdit* hostEdit_;
    QSpinBox* portSpin_;
    QLineEdit* nameEdit_;
    bool hosting_;
};

#endif // APP_DIALOGS_MULTIPLAYERDIALOG_H
