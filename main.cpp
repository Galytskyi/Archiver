#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Description of Organization
    //
    a.setApplicationName("Archiver");
    a.setOrganizationName("Home");

    // Get work path from cmd line
    //
    QDir dir;
    if (argc >= 2)
        dir = QDir(argv[1]);

    if (!dir.exists())
        dir = QDir::currentPath();

    QString path = dir.absolutePath();

    // MainWindow
    //
    MainWindow w(path);
    w.show();

    return a.exec();
}
