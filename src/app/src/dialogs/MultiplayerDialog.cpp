#include "app/dialogs/MultiplayerDialog.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QNetworkInterface>
#include <QRadioButton>
#include <QVBoxLayout>

static QString localIP() {
    // Find the first non-loopback IPv4 address
    const auto& ifaces = QNetworkInterface::allInterfaces();
    for (const auto& iface : ifaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack))
            continue;
        if (!iface.flags().testFlag(QNetworkInterface::IsUp))
            continue;
        const auto& entries = iface.addressEntries();
        for (const auto& entry : entries) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                return entry.ip().toString();
            }
        }
    }
    return "127.0.0.1";
}

MultiplayerDialog::MultiplayerDialog(QWidget* parent)
    : QDialog(parent)
    , hosting_(true)
{
    setWindowTitle(tr("Multiplayer"));
    setMinimumWidth(380);

    auto* mainLayout = new QVBoxLayout(this);

    // Mode selection
    auto* modeGroup = new QGroupBox(tr("Mode"), this);
    auto* modeLayout = new QVBoxLayout(modeGroup);
    auto* hostRadio = new QRadioButton(tr("Host Game"), this);
    auto* joinRadio = new QRadioButton(tr("Join Game"), this);
    hostRadio->setChecked(true);
    modeLayout->addWidget(hostRadio);
    modeLayout->addWidget(joinRadio);
    mainLayout->addWidget(modeGroup);

    // IP hint
    auto* ipHint = new QLabel(
        tr("Your IP: %1  (tell this to the other player)").arg(localIP()), this);
    ipHint->setStyleSheet("color: #4CAF50; font-weight: bold;");
    mainLayout->addWidget(ipHint);

    // Form
    auto* form = new QFormLayout();

    QString defaultHost = localIP();
    hostEdit_ = new QLineEdit(defaultHost, this);
    hostEdit_->setEnabled(false);
    form->addRow(tr("Host Address:"), hostEdit_);

    portSpin_ = new QSpinBox(this);
    portSpin_->setRange(1024, 65535);
    portSpin_->setValue(16666);
    form->addRow(tr("Port:"), portSpin_);

    nameEdit_ = new QLineEdit("Player", this);
    form->addRow(tr("Player Name:"), nameEdit_);

    mainLayout->addLayout(form);

    connect(hostRadio, &QRadioButton::toggled, this, [this, hostEdit = hostEdit_](bool checked) {
        hosting_ = checked;
        hostEdit->setEnabled(!checked);
    });

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

bool MultiplayerDialog::isHosting() const { return hosting_; }
QString MultiplayerDialog::hostAddress() const { return hostEdit_->text(); }
quint16 MultiplayerDialog::port() const {
    return static_cast<quint16>(portSpin_->value());
}
QString MultiplayerDialog::playerName() const { return nameEdit_->text(); }
