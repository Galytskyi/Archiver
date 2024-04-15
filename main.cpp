#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Organization description
    //
    a.setApplicationName("Archiver");
    a.setOrganizationName("Home");

    // get work path from cmd line
    //
    QDir dir;
    if (argc >= 2)
        dir = QDir(argv[1]);

    if (!dir.exists())
        dir = QDir::currentPath();

    QString path = dir.absolutePath();


    if(!dir.exists())
        path = QDir::currentPath();

    // MainWindow
    //
    MainWindow w(path);
    w.show();

    return a.exec();
}
