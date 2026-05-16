#include <QApplication>
#include <QLabel>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QLabel label("Snake Arena - build OK");
    label.show();
    return app.exec();
}
