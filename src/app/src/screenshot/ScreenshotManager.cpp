#include "app/screenshot/ScreenshotManager.h"
#include <QDateTime>
#include <QDir>
#include <QPixmap>
#include <QWidget>

ScreenshotManager::ScreenshotManager(QObject* parent)
    : QObject(parent)
{
}

bool ScreenshotManager::capture(QWidget* widget, const QString& dir,
                                 QString* outPath) {
    if (!widget) return false;

    QPixmap pixmap = widget->grab();
    if (pixmap.isNull()) return false;

    QDir().mkpath(dir);
    ++counter_;
    QString filename = QString("snake_%1_%2.png")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
        .arg(counter_, 3, 10, QChar('0'));
    QString path = QDir(dir).filePath(filename);

    if (pixmap.save(path, "PNG")) {
        if (outPath) *outPath = path;
        return true;
    }
    return false;
}
