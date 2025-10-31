#include <QApplication>
#include <QWidget>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QWidget w;
    w.resize(800, 600);
    w.setWindowTitle("cpp-uml-generator");
    w.show();
    return app.exec();
}