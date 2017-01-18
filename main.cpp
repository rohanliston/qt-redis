#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "QMLRedisInterface.h"
#include "CppRedisTest.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<QMLRedisInterface>("Redis", 1, 0, "RedisInterface");

    CppRedisTest test;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
