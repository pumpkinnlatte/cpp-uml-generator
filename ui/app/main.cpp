#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QMainWindow w;
    QLabel *label = new QLabel("cpp-to-uml GUI (placeholder)", &w);
    w.setCentralWidget(label);
    w.resize(800, 600);
    w.show();
    return QApplication::exec();
}