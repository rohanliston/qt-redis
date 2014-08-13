#ifndef QMLREDISINTERFACE_H
#define QMLREDISINTERFACE_H

#include <QQuickItem>
#include <QDebug>
#include "RedisInterface.h"

class QMLRedisInterface : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString  serverUrl            READ serverUrl            WRITE setServerUrl            NOTIFY serverUrlChanged           )
    Q_PROPERTY(QVariant subscribedProperties READ subscribedProperties WRITE setSubscribedProperties NOTIFY subscribedPropertiesChanged)
    Q_PROPERTY(QVariant publishedProperties  READ publishedProperties  WRITE setPublishedProperties  NOTIFY publishedPropertiesChanged )
    Q_PROPERTY(QVariant subscribedEvents     READ subscribedEvents     WRITE setSubscribedEvents     NOTIFY subscribedEventsChanged    )
    Q_PROPERTY(QVariant publishedEvents      READ publishedEvents      WRITE setPublishedEvents      NOTIFY publishedEventsChanged     )

public:

    explicit QMLRedisInterface(QQuickItem *parent = 0);

    QString serverUrl() const;
    QVariant subscribedProperties() const;
    QVariant publishedProperties() const;
    QVariant subscribedEvents() const;
    QVariant publishedEvents() const;

    Q_INVOKABLE QVariant get(const QString& key) const;
    Q_INVOKABLE void get(const QString& key, QJSValue callback) const;
    Q_INVOKABLE bool subscribeToEvent(const QString& remoteEventName, const QString& localMethodName);

    Q_INVOKABLE void init();

signals:

    void serverUrlChanged(const QString& value);
    void subscribedPropertiesChanged(const QVariant& value);
    void publishedPropertiesChanged(const QVariant& value);
    void subscribedEventsChanged(const QVariant& value);
    void publishedEventsChanged(const QVariant& value);

public slots:

    void set(const QString& key, const QVariant& value);

    void setServerUrl(const QString& value);
    void setSubscribedProperties(const QVariant& value);
    void setPublishedProperties(const QVariant& value);
    void setSubscribedEvents(const QVariant& value);
    void setPublishedEvents(const QVariant& value);

private:

    QString _serverUrl;
    QVariant _subscribedProperties;
    QVariant _publishedProperties;
    QVariant _subscribedEvents;
    QVariant _publishedEvents;

    RedisInterface* _redisInterface;
};

QML_DECLARE_TYPE(QMLRedisInterface)

#endif // QMLREDISINTERFACE_H
