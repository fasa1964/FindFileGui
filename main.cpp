#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("FindFileGui");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("qtexample@findfilegui");
    app.setOrganizationDomain("qtexample@findfilegui.com");

    MainWindow w;
    w.show();
    return app.exec();
}
