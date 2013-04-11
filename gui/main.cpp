#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("mit");
    QCoreApplication::setOrganizationDomain("mit.edu");
    QCoreApplication::setApplicationName("FoamCut");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
