#include <QApplication>
#include <QFile>
#include "AppShell.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QFile qss(":/style/main.qss");
    if (qss.open(QFile::ReadOnly)) {
        app.setStyleSheet(qss.readAll());
        qss.close();
    } else {
        LOG_WARN("main", "Failed to load QSS stylesheet");
    }

    AppShell shell;
    shell.show();

    return app.exec();
}
