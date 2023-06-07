#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    auto *m = new MainWindow();
    m->show();
    exit( QApplication::exec());
}