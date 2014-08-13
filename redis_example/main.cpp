#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "CppRedisTest.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QtQuick2ApplicationViewer viewer;

    CppRedisTest cRedisTest;

    viewer.setMainQmlFile(QStringLiteral("qml/redis_interface/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
