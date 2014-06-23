#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "QMLRedisInterface.h"
#include "CppRedisTest.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<QMLRedisInterface>("Redis", 1, 0, "RedisInterface");

    CppRedisTest test;

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/redis_client/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
