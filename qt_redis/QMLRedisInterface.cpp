#include "QMLRedisInterface.h"

/*!
    \mainclass
    \class QMLRedisInterface
    \inmodule RedisInterface
    \brief A simple wrapper class that enables a RedisInterface to be instantiated as a QML Item.

    This QQuickItem-based wrapper provides call-throughs to a RedisInterface instance, which
    provides a common interface for QObject-based objects to communicate via Redis + HTTP.

    Subscribed/published events and properties are defined in a declarative fashion using the \c{variant} properties
    \l{subscribedEvents}, \l{publishedEvents}, \l{subscribedProperties}, and \l{publishedProperties}.

    Example:
    \code
    // Remote properties we wish to pull from Redis automatically
    subscribedProperties: [
        { remote: "remote:published:double", local: "localSubscribedDouble" },
        { remote: "remote:published:string", local: "localSubscribedString" }
    ]

    // Local properties we wish to push to Redis automatically
    publishedProperties: [
        { local: "localPublishedDouble", remote: "remote:subscribed:double" },
        { local: "localPublishedString", remote: "remote:subscribed:string" }
    ]

    // Remote events to which we wish to subscribe, triggering local signals/methods automatically
    subscribedEvents: [
        { remote: "remote:signal:trigger", local: "localSignal"   },
        { remote: "remote:method:trigger", local: "localMethod"   },
        { remote: "remote:wildcard*",      local: "localWildcard" }
    ]

    // Local events (signals) we wish to publish to Redis when they are emitted
    publishedEvents: [
        { local: "publishedSignal1", remote: "remote:subscribed:event1" },
        { local: "publishedSignal2", remote: "remote:subscribed:event2" }
    ]

    // Local properties that are 'pulled' from Redis
    property double localSubscribedDouble
    property string localSubscribedString

    // Local properties that are 'pushed' to Redis
    property double localPublishedDouble: 0.00
    property string localPublishedString: "."

    // Signals we wish to publish to Redis
    signal publishedSignal1
    signal publishedSignal2

    // Signal to trigger when "remote:signal:trigger" is emitted by Redis
    signal localSignal
    onLocalSignal: {
        console.log("(QML) localSignal() emitted");
    }

    // Method to invoke when "remote:method:trigger" is emitted by Redis
    function localMethod() {
        console.log("(QML) localMethod() called");
    }

    // Method to invoke when "remote:wildcard*" is emitted by Redis
    function localWildcard() {
        console.log("(QML) localWildcard() called");
    }
    \endcode

    \sa RedisInterface
*/

QMLRedisInterface::QMLRedisInterface(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QVariant QMLRedisInterface::subscribedEvents() const
{
    return _subscribedEvents;
}

QVariant QMLRedisInterface::get(const QString &key) const
{
    if(this->isComponentComplete())
        return _redisInterface->get(key);

    return QVariant();
}

void QMLRedisInterface::get(const QString &key, QJSValue callback) const
{
    if(this->isComponentComplete())
        _redisInterface->get(key, callback);
}

bool QMLRedisInterface::subscribeToEvent(const QString& remoteEventName, const QString& localMethodName)
{
    if(this->isComponentComplete())
        return _redisInterface->subscribeToEvent(remoteEventName, localMethodName);

    return false;
}

void QMLRedisInterface::init()
{
    _redisInterface = new RedisInterface(serverUrl(), this);

    // Subscribe to events.
    QListIterator<QVariant> subscribedEventsIter = subscribedEvents().toList();
    while(subscribedEventsIter.hasNext())
    {
        QVariantMap element = subscribedEventsIter.next().toMap();

        QString remoteEventName = element.value("remote").toString();
        QString localMethodName = QString(element.value("local").toString());

        _redisInterface->subscribeToEvent(remoteEventName, localMethodName);
    }

    // Publish events.
    QListIterator<QVariant> publishedEventsIter = publishedEvents().toList();
    while(publishedEventsIter.hasNext())
    {
        QVariantMap element = publishedEventsIter.next().toMap();

        QString localSignalName = element.value("local").toString();
        QString remoteEventName = element.value("remote").toString();

        _redisInterface->publishEvent(localSignalName, remoteEventName);
    }

    // Subscribe to properties.
    QListIterator<QVariant> subscribedPropertiesIter = subscribedProperties().toList();
    while(subscribedPropertiesIter.hasNext())
    {
        QVariantMap element = subscribedPropertiesIter.next().toMap();

        QString remotePropertyName = element.value("remote").toString();
        QString localPropertyName = element.value("local").toString();

        _redisInterface->subscribeToProperty(remotePropertyName, localPropertyName);
    }

    // Publish properties.
    QListIterator<QVariant> publishedPropertiesIter = publishedProperties().toList();
    while(publishedPropertiesIter.hasNext())
    {
        QVariantMap element = publishedPropertiesIter.next().toMap();

        QString remotePropertyName = element.value("remote").toString();
        QString localPropertyName = element.value("local").toString();

        _redisInterface->publishProperty(localPropertyName, remotePropertyName);
    }
}

QString QMLRedisInterface::serverUrl() const
{
    return _serverUrl;
}

void QMLRedisInterface::set(const QString &key, const QVariant &value)
{
    if(this->isComponentComplete())
        _redisInterface->set(key, value);
}

QVariant QMLRedisInterface::publishedProperties() const
{
    return _publishedProperties;
}

QVariant QMLRedisInterface::subscribedProperties() const
{
    return _subscribedProperties;
}

QVariant QMLRedisInterface::publishedEvents() const
{
    return _publishedEvents;
}

void QMLRedisInterface::setSubscribedEvents(const QVariant &value)
{
    if(_subscribedEvents != value)
    {
        _subscribedEvents = value;
        emit subscribedEventsChanged(value);
    }
}

void QMLRedisInterface::setPublishedProperties(const QVariant &value)
{
    if(_publishedProperties != value)
    {
        _publishedProperties = value;
        emit publishedPropertiesChanged(value);
    }
}

void QMLRedisInterface::setSubscribedProperties(const QVariant& value)
{
    if(_subscribedProperties != value)
    {
        _subscribedProperties = value;
        emit subscribedPropertiesChanged(value);
    }
}

void QMLRedisInterface::setPublishedEvents(const QVariant& value)
{
    if(_publishedEvents != value)
    {
        _publishedEvents = value;
        emit publishedEventsChanged(value);
    }
}

void QMLRedisInterface::setServerUrl(const QString& value)
{
    if(_serverUrl != value)
    {
        _serverUrl = value;
        emit serverUrlChanged(value);
    }
}
