#ifndef REDISINTERFACE_H
#define REDISINTERFACE_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMetaMethod>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <iostream>
#include <QJSEngine>
#include <QJSValue>
#include <QObjectUserData>
#include <QEventLoop>
#include <QDebug>
#include <stdexcept>

class RedisInterface : public QObject
{
    Q_OBJECT

public:

    /** Constructor. Requires a valid QObject-based parent to map events to/from. */
    RedisInterface(QString serverUrl, QObject* parent);

    /** Convenience methods for retrieving methods/signals/slots/properties from a given QObject's meta-object. */
    static QMetaMethod getMethod(QObject* object, QString signature);
    static QMetaMethod getSignal(QObject* object, QString signature);
    static QMetaMethod getSlot(QObject* object, QString signature);
    static QMetaProperty getProperty(QObject* object, QString propertyName);

public slots:

    /** Subscribes to the given Redis event, causing localMethodName to be called automatically. */
    bool subscribeToEvent(QString remoteEventName, QString localMethodName);

    /** Publishes the given local signal, generating the given Redis event automatically. */
    void publishEvent(QString localSignalName, QString remoteEventName);

    /** Subscribes to the given Redis property, causing localPropertyName to be updated automatically. */
    void subscribeToProperty(QString remotePropertyName, QString localPropertyName);

    /** Publishes the given local property, causing remotePropertyName to be updated automatically on Redis. */
    void publishProperty(QString localPropertyName, QString remotePropertyName);

    /** SETs the given Redis property to the given value. */
    void set(QString key, const QVariant& value);

    /** Performs a synchronous (thread-blocking) GET request for the given Redis key. */
    QVariant get(QString key) const;

    /** Performs an asynchronous GET request, calling the given JavaScript callback upon completion. */
    void get(QString key, QJSValue callback) const;

    /** Performs an asynchronous GET request, calling the given C++ method upon completion. */
    void get(QString key, QMetaMethod callback) const;

    /** Performs a single-shot PUBLISH event to Redis, with the given event name and value. */
    void publish(QString remoteEventName, QVariant value);

private slots:

    /** Adds a subscription to the given remote event, hooking it up to the given method belonging to the given QObject subclass. */
    void addEventSubscription(QString remoteEventName, QObject *targetObject, QMetaMethod targetMethod);

    /** Private handler slots to catch remote and local events. */
    void handleSubscribedEvent();
    void handlePublishedEvent();
    void handleSubscribedPropertyUpdate();
    void handlePublishedPropertyUpdate();
    void handleGetRequestResponse_JavaScript();
    void handleGetRequestResponse_MetaMethod();

private:

    /** Packages up a JavaScript callback such that it can be tacked onto a QNetworkReply for later invocation. */
    class JavaScriptCallback : public QObjectUserData
    {
    public:
        JavaScriptCallback(QJSValue callback) : QObjectUserData(), callback(callback) {}
        QJSValue callback;
    };

    /** Packages up a QMetaMethod callback such that it can be tacked onto a QNetworkReply for later invocation. */
    class MetaMethodCallback: public QObjectUserData
    {
    public:
        MetaMethodCallback(QObject* requesterObj, QString requestedKey, QMetaMethod callbackMethod) :
            QObjectUserData(),
            requester(requesterObj),
            key(requestedKey),
            callback(callbackMethod)
        {}

        QObject* requester;
        QString key;
        QMetaMethod callback;
    };

    /** URL of the Redis HTTP (webdis) server (eg. "http://localhost:7379/") */
    QString _serverUrl;

    /** Object responsible for making network requests to Redis. */
    QNetworkAccessManager* _networkInterface;

    /** Mapping of Redis events to local methods on the parent object. */
    QMap<QString, QMetaMethod> _subscribedEvents;

    /** Mapping of parent signals to Redis event names. */
    QMap<QString, QString> _publishedEvents;

    /** Mapping of Redis properties to local Q_PROPERTYs on the parent object. */
    QMap<QString, QMetaProperty> _subscribedProperties;

    /** Mapping of parent property names to Redis properties. */
    QMap<QString, QString> _publishedProperties;
};

#endif // REDISINTERFACE_H
