#ifndef APP_SCREENSHOT_SCREENSHOTMANAGER_H
#define APP_SCREENSHOT_SCREENSHOTMANAGER_H

#include <QObject>
#include <QString>

class QWidget;

class ScreenshotManager : public QObject {
    Q_OBJECT

public:
    explicit ScreenshotManager(QObject* parent = nullptr);

    bool capture(QWidget* widget, const QString& dir,
                 QString* outPath = nullptr);

private:
    int counter_ = 0;
};

#endif // APP_SCREENSHOT_SCREENSHOTMANAGER_H
