#include <QApplication>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("COSMOS Node Editor");
    app.setOrganizationName("HSFL");

    // Nodes directory: optional first arg, else <exedir>/../cosmos/nodes
    QString nodesDir;
    if (argc > 1) {
        nodesDir = argv[1];
    } else {
        nodesDir = QDir::cleanPath(
            QCoreApplication::applicationDirPath() + "/../cosmos/nodes");
    }

    MainWindow w(nodesDir);
    w.show();
    return app.exec();
}
