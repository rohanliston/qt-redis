#include <QApplication>
#include <QQmlApplicationEngine>
#include "CppRedisTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CppRedisTest test;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
