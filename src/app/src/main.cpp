#include <QApplication>
#include "app/MainWindow.h"
#include "core/engine/Direction.h"
#include "core/logging/Logger.h"
#include "core/logging/ConsoleSink.h"
#include "core/logging/FileSink.h"

#include <QDateTime>

int main(int argc, char* argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    qRegisterMetaType<core::Direction>("core::Direction");

    // Separate log per process by timestamp
    QString logName = QString("qtsnake_%1.log")
        .arg(QDateTime::currentDateTime().toString("HHmmsszzz"));
    core::Logger::instance().addSink(
        std::make_unique<core::ConsoleSink>());
    core::Logger::instance().addSink(
        std::make_unique<core::FileSink>(logName.toStdString()));

    QApplication app(argc, argv);
    app.setApplicationName("QtSnake Lab");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    window.show();

    return app.exec();
}
