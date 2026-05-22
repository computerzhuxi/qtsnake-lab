#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setObjectName("settingsDialog");
    setFixedSize(340, 180);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setModal(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    auto* icon = new QLabel("!", this);
    icon->setObjectName("dialogIcon");
    icon->setAlignment(Qt::AlignCenter);
    layout->addWidget(icon);
    layout->addSpacing(10);

    auto* title = new QLabel(tr("设 置 已 更 改"), this);
    title->setObjectName("dialogTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    layout->addSpacing(6);

    auto* desc = new QLabel(tr("将在 下一局 生效"), this);
    desc->setObjectName("dialogDesc");
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);
    layout->addSpacing(18);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);

    auto* btnRestart = new QPushButton(tr("重新开始"), this);
    btnRestart->setObjectName("dialogPrimary");
    btnRow->addWidget(btnRestart);

    auto* btnLater = new QPushButton(tr("稍后再说"), this);
    btnLater->setObjectName("dialogSecondary");
    btnRow->addWidget(btnLater);

    layout->addLayout(btnRow);

    connect(btnRestart, &QPushButton::clicked, this, [this]() {
        emit restartClicked();
        accept();
    });

    connect(btnLater, &QPushButton::clicked, this, [this]() {
        emit laterClicked();
        reject();
    });
}
