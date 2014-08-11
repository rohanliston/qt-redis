#ifndef CPPREDISTEST_H
#define CPPREDISTEST_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "../QRedisLib/RedisInterface.h"

class CppRedisTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString propertyFromCpp   READ propertyFromCpp   WRITE setPropertyFromCpp   NOTIFY propertyFromCppChanged  )
    Q_PROPERTY(QString propertyFromRedis READ propertyFromRedis WRITE setPropertyFromRedis NOTIFY propertyFromRedisChanged)

public:
    explicit CppRedisTest(QObject *parent = 0);

    QString propertyFromCpp() const;
    QString propertyFromRedis() const;

signals:

    void eventFromCpp();
    void propertyFromCppChanged(QString arg);
    void propertyFromRedisChanged(QString arg);

public slots:

    void eventFromRedis();
    void setPropertyFromCpp(QString arg);
    void setPropertyFromRedis(QString arg);

    void update();

    void getRequestResponse(QString key, QVariant value);

private:

    RedisInterface* _redisInterface;
    QTimer* _timer;

    QString _propertyFromCpp;
    QString _propertyFromRedis;
};

#endif // CPPREDISTEST_H
