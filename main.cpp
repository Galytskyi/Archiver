#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Description of Organization
    //
    a.setApplicationName("Archiver");
    a.setOrganizationName("Home");

    // Get work path from cmd line
    //
    QString path = Arch::getWorkDir(argc, argv).c_str();

    // MainWindow
    //
    MainWindow w(path);
    w.show();

    return a.exec();
}
