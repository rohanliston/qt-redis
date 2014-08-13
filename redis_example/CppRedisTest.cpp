#include "CppRedisTest.h"

CppRedisTest::CppRedisTest(QObject *parent) :
    QObject(parent),
    _redisInterface(new RedisInterface("http://localhost:7379/", this)),
    _timer(new QTimer())
{
    _redisInterface->subscribeToEvent("event:from:redis", "eventFromRedis");
    _redisInterface->publishEvent("eventFromCpp", "event:from:cpp");
    _redisInterface->publishProperty("propertyFromCpp", "property:from:cpp");
    _redisInterface->subscribeToProperty("property:from:redis", "propertyFromRedis");

    _timer->setInterval(1000);
    _timer->setSingleShot(false);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
    _timer->start();
}

QString CppRedisTest::propertyFromCpp() const
{
    return _propertyFromCpp;
}

QString CppRedisTest::propertyFromRedis() const
{
    return _propertyFromRedis;
}

void CppRedisTest::eventFromRedis()
{
    qDebug() << "(C++) eventFromRedis() called";
}

void CppRedisTest::setPropertyFromCpp(QString arg)
{
    if (_propertyFromCpp != arg) {
        _propertyFromCpp = arg;
        emit propertyFromCppChanged(arg);
    }
}

void CppRedisTest::setPropertyFromRedis(QString arg)
{
    if (_propertyFromRedis != arg) {
        _propertyFromRedis = arg;
        emit propertyFromRedisChanged(arg);
    }
}

void CppRedisTest::update()
{
    emit eventFromCpp();
    setPropertyFromCpp(QString(rand()));
    _redisInterface->get("test:meta:value", RedisInterface::getSlot(this, "getRequestResponse(QString, QVariant)"));
}

void CppRedisTest::getRequestResponse(QString key, QVariant value)
{
    qDebug() << "(C++) Get request response:" << key << "==" << value;
}
