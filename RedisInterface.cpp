#include "RedisInterface.h"

/*!
    \mainclass
    \class RedisInterface
    \inmodule RedisInterface
    \brief Provides a common interface for QObject-based objects to communicate via Redis + HTTP.

    Redis is a networked, in-memory, key-value data store with an inbuilt publish/subscribe messaging system. The RedisInterface class
    enables binding of Redis values to/from Qt properties, and Redis events to/from Qt methods/signals/slots.

    When a \c{RedisInterface} object is instantiated, any Qt properties of the parent class defined using the \c{Q_PROPERTY} macro are available
    to be synchronised (in either direction) with corresponding values on the Redis server. Similarly, any Qt \c{signals} can be set to automatically
    \c{PUBLISH} to Redis events, and any \c{slots}/\c{signals}/ordinary methods can be set up to receive Redis events via \c{SUBSCRIBE}/\c{PSUBSCRIBE}.

    In addition to event/property binding, the \c{RedisInterface} supports a nominal set of 'once-off' commands such as \c{GET},\c{SET}, and \c{PUBLISH}.
    This set of commands will be expanded in the future as required.

    A QML wrapper is provided by the QMLRedisInterface class.

    \sa QMLRedisInterface
*/

/*!
 * \brief Constructor. Enables this object to interact with the Redis (webdis) server at \a{serverUrl}
 * and map Redis events/properties to \c{signals}/\c{slots}/properties on the given \c{QObject}-based \a{parent} (and vice versa).
 */
RedisInterface::RedisInterface(QString serverUrl, QObject *parent) :
    QObject(parent),
    _serverUrl(serverUrl),
    _networkInterface(new QNetworkAccessManager(this))
{
    // Make sure Redis server URL ends with a slash.
    if(!_serverUrl.endsWith("/"))
        _serverUrl.append("/");

    // Fail hard if no parent is given.
    if(parent == NULL)
        throw std::runtime_error("RedisInterface::RedisInterface(): RedisInterface constructor must be passed a non-null QObject-based parent!");
}

/*!
 * \brief Inspects the meta-object of the given \a{object} and returns the method with the given \a{signature}.
 * Returns an invalid \l{QMetaMethod} if the method could not be found.
 */
QMetaMethod RedisInterface::getMethod(QObject *object, QString signature)
{
    if(object)
        return object->metaObject()->method(object->metaObject()->indexOfMethod(object->metaObject()->normalizedSignature(signature.toStdString().c_str())));
    else
        std::cerr << "RedisInterface::getMethod(): Can't find method on NULL!";

    return QMetaMethod();
}

/*!
 * \brief Inspects the meta-object of the given \a{object} and returns the \c{signal} with the given \a{signature}.
 * Returns an invalid \l{QMetaMethod} if the method could not be found, or if the method is not a \c{signal}.
 */
QMetaMethod RedisInterface::getSignal(QObject *object, QString signature)
{
    if(object)
        return object->metaObject()->method(object->metaObject()->indexOfSignal(object->metaObject()->normalizedSignature(signature.toStdString().c_str())));
    else
        std::cerr << "RedisInterface::getSignal(): Can't find signal on NULL!";

    return QMetaMethod();
}

/*!
 * \brief Inspects the meta-object of the given \a{object} and returns the \c{signal} with the given \a{signature}.
 * Returns an invalid \l{QMetaMethod} if the method could not be found, or if the method is not a \c{slot}.
 */
QMetaMethod RedisInterface::getSlot(QObject *object, QString signature)
{
    if(object)
        return object->metaObject()->method(object->metaObject()->indexOfSlot(object->metaObject()->normalizedSignature(signature.toStdString().c_str())));
    else
        std::cerr << "RedisInterface::getSlot(): Can't find slot on NULL!";

    return QMetaMethod();
}

/*!
 * \brief Inspects the meta-object of the given \a{object} and returns the \c{Q_PROPERTY} with the given \a{propertyName}.
 * Returns an invalid \l{QMetaProperty} if the property could not be found.
 */
QMetaProperty RedisInterface::getProperty(QObject *object, QString propertyName)
{
    if(object)
        return object->metaObject()->property(object->metaObject()->indexOfProperty(propertyName.toStdString().c_str()));
    else
        std::cerr << "RedisInterface::getProperty(): Can't find property on NULL!";

    return QMetaProperty();
}

/*!
 * \brief Adds a subscription to the Redis event \a{remoteEventName}, which will cause \a{localMethodName} (if valid) to be invoked
 * each time \a{remoteEventName} occurs.
 */
bool RedisInterface::subscribeToEvent(QString remoteEventName, QString localMethodName)
{
    // Append parentheses if missing.
    if(!localMethodName.endsWith("()"))
        localMethodName += "()";

    // Locate target method in parent's meta-object.
    QMetaMethod localMethod = RedisInterface::getMethod(parent(), localMethodName);

    if(localMethod.isValid())
    {
        // Set remoteEventName to call localMethod each time it is triggered.
        addEventSubscription(remoteEventName, parent(), localMethod);
        _subscribedEvents.insert(remoteEventName, localMethod);

        return true;
    }
    else
    {
        std::cerr << "RedisInterface::subscribeToEvent(): Target method " << localMethodName.toStdString() << " invalid or not found!" << std::endl;
        return false;
    }
}

/*!
 * \brief Adds a publication of \a{localSignalName}, which (if valid) will cause a \a{remoteEventName} event to be sent to Redis
 * each time the local signal is emitted is emitted.
 */
void RedisInterface::publishEvent(QString localSignalName, QString remoteEventName)
{
    // Append parentheses if missing.
    if(!localSignalName.endsWith("()"))
        localSignalName.append("()");

    // Locate the source signal in parent's meta-obeject.
    QMetaMethod localSignal = RedisInterface::getSignal(parent(), localSignalName);

    if(localSignal.isValid())
    {
        qDebug() << "[RedisInterface] Connecting local signal" << localSignalName << "to remote event" << remoteEventName;

        // Set handlePublishedEvent() to be called each time localSignalName is emitted..
        QMetaMethod publishSlot = RedisInterface::getSlot(this, "handlePublishedEvent()");
        connect(parent(), localSignal, this, publishSlot);

        _publishedEvents.insert(localSignalName, remoteEventName);
    }
    else
    {
        std::cerr << "RedisInterface::publishEvent(): Source signal " << localSignalName.toStdString() << " invalid or not found!" << std::endl;
    }
}

/*!
 * \brief Adds a subscription to the Redis property \a{remotePropertyName}, which will cause the value of \a{localPropertyName} to be
 * automatically updated each time the remote property changes.
 */
void RedisInterface::subscribeToProperty(QString remotePropertyName, QString localPropertyName)
{
    qDebug() << "[RedisInterface] Mapping remote property" << remotePropertyName << "to local property" << localPropertyName;

    // Set handleSubscribedPropertyUpdate() to be called each time the '_changed' event for remotePropertyName is received.
    QMetaMethod propertyUpdateSlot = RedisInterface::getSlot(this, "handleSubscribedPropertyUpdate()");
    addEventSubscription(remotePropertyName + "_changed", this, propertyUpdateSlot);

    _subscribedProperties.insert(remotePropertyName, RedisInterface::getProperty(parent(), localPropertyName));
}

/*!
 * \brief Adds a publication of the local property \a{localPropertyName}, which will cause the Redis property \a{remotePropertyName} to be
 * automatically updated each time the local property changes. In addition, a "remotePropertyName_changed" Redis event will be generated to notify
 * any subscribers to the updated property.
 */
void RedisInterface::publishProperty(QString localPropertyName, QString remotePropertyName)
{
    // Locate the local property on the parent's meta-object.
    QMetaProperty property = RedisInterface::getProperty(parent(), localPropertyName);

    if(property.isValid() && property.hasNotifySignal())
    {
        // Set the property's NOTIFY signal to call handlePublishedPropertyUpdate() automatically.
        QMetaMethod notifySignal = property.notifySignal();
        QMetaMethod setSlot = RedisInterface::getSlot(this, "handlePublishedPropertyUpdate()");

        qDebug() << "[RedisInterface] Mapping local property" << localPropertyName << "to remote property" << remotePropertyName << "(via notify signal" << notifySignal.name() + "())";
        connect(parent(), notifySignal, this, setSlot);
        _publishedProperties.insert(localPropertyName, remotePropertyName);
    }
    else
    {
        std::cerr << "[RedisInterface] Error: Property " << localPropertyName.toStdString() << " is invalid or has no NOTIFY signal!" << std::endl;
    }
}

/*!
 * \brief Subscribes to the Redis event \a{remoteEventName}, connecting it to the \a{targetMethod} method belonging to \a{targetObject}.
 */
void RedisInterface::addEventSubscription(QString remoteEventName, QObject* targetObject, QMetaMethod targetMethod)
{
    if(targetObject != NULL && targetMethod.isValid() && targetMethod.enclosingMetaObject() == targetObject->metaObject())
    {
        // Use pattern subscribe for remote events containing wildcards.
        QString subscribeCommand = remoteEventName.contains("*") ? "PSUBSCRIBE" : "SUBSCRIBE";

        qDebug() << "[RedisInterface] Connecting remote event" << remoteEventName << "to local method" << targetMethod.name() << "using" << subscribeCommand;

        // Make the subscription request via Redis (over HTTP via webdis).
        QNetworkReply* reply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + subscribeCommand + "/" + remoteEventName)));
        QMetaMethod readyRead = RedisInterface::getSignal(reply, "readyRead()");
        connect(reply, readyRead, targetObject, targetMethod);
        connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    }
    else
    {
        if(targetObject == NULL)
        {
            std::cerr << "[RedisInterface] addEventSubscription(): targetObject must not be NULL!" << std::endl;
        }
        else
        {
            if(!targetMethod.isValid())
                std::cerr << "[RedisInterface] addEventSubscription(): targetMethod is invalid!" << std::endl;

            if(targetMethod.enclosingMetaObject() != targetObject->metaObject())
                std::cerr << "[RedisInterface] addEventSubscription(): targetMethod does not belong to targetObject!" << std::endl;
        }
    }
}

/*!
 * \brief Handles an incoming Redis event, invoking any corresponding methods/signals/slots that have been bound to the event.
 */
void RedisInterface::handleSubscribedEvent()
{
    // Retrieve the network reply.
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try {
        QJsonObject doc = QJsonDocument::fromJson(reply->readAll()).object();

        // Subscription message format is {"SUBSCRIPTION":["message/pmessage","message_name","message_value"]}.
        QJsonArray data = doc.value(doc.keys().at(0)).toArray();
        QString eventType = data.at(0).toString();

        // Invoke the method if the event is a proper message (as opposed to the initial subscription reply).
        if(eventType == "message" || eventType == "pmessage")
        {
            QString eventName = data.at(1).toString();
            _subscribedEvents.value(eventName).invoke(parent());
        }
    }
    catch(std::exception& e)
    {
        if(reply)
            std::cerr << "RedisInterface::receiveRemoteEvent(): Error: " << reply->errorString().toStdString() << std::endl;
        else
            std::cerr << "RedisInterface::receiveRemoteEvent(): Error: Network reply is NULL!" << std::endl;
    }
}

/*!
 * \brief Handles a published event (signal) being emitted, in turn publishing any Redis events that have been bound to the event.
 */
void RedisInterface::handlePublishedEvent()
{
    // Retrieve the signal sender.
    QString senderSignal = parent()->metaObject()->method(senderSignalIndex()).methodSignature();

    qDebug() << "[RedisInterface] Publishing" << senderSignal << "--->" << _publishedEvents.value(senderSignal);
    publish(_publishedEvents.value(senderSignal), senderSignal);
}

/*!
 * \brief Handles an incoming 'changed' event for a subscribed property. Sets the corresponding local property to the new value.
 */
void RedisInterface::handleSubscribedPropertyUpdate()
{
    // Retrieve the network reply.
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        // Parse the JSON response.
        QJsonObject doc = QJsonDocument::fromJson(reply->readAll()).object();

        // Subscription message format is {"SUBSCRIPTION":["message/pmessage","message_name","message_value"]}.
        QJsonArray data = doc.value(doc.keys().at(0)).toArray();
        QString eventType = data.at(0).toString();

        // Invoke the method if the event is a proper message (as opposed to the initial subscription reply).
        if(eventType == "message" || eventType == "pmessage")
        {
            QString eventName = data.at(1).toString();

            if(eventName.endsWith("_changed"))
            {
                QString propertyName = eventName.left(eventName.length() - 8);
                QVariant value = data.at(2).toVariant();
                qDebug() << "[RedisInterface] Remote property" << propertyName << "changed to" << value;
                _subscribedProperties.value(propertyName).write(parent(), value);
            }
            else
            {
                std::cerr << "[RedisInterface] Error: Unexpected property changed message format: " << eventName.toStdString() << std::endl;
            }
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "[RedisInterface] handleSubscribedPropertyUpdate(): Error: " << e.what();
    }
}

/*!
 * \brief Handles a published property update, in turn updating the corresponding Redis value.
 */
void RedisInterface::handlePublishedPropertyUpdate()
{
    // Retrieve the name of the NOTIFY signal that triggered this slot.
    QString senderSignal = parent()->metaObject()->method(senderSignalIndex()).name();

    if(senderSignal.endsWith("Changed"))
    {
        QString localPropertyName = senderSignal.left(senderSignal.length() - QString("Changed").length());
        QVariant newValue = parent()->property(localPropertyName.toStdString().c_str());

        // Update the Redis value.
        set(_publishedProperties.value(localPropertyName), newValue);
    }
    else
    {
        std::cerr << "[RedisInterface] handlePublishedPropertyUpdate(): Unexpected sender signal " << senderSignal.toStdString() << std::endl;
    }
}

/*!
 * \brief Handles an asynchronous GET request response, calling the appropriate JavaScript callback.
 */
void RedisInterface::handleGetRequestResponse_JavaScript()
{
    // Retrieve the network reply.
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        // Parse the JSON response.
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        // Find the JavaScriptCallback we tacked on earlier, retrieve the callback, and call it with the value we requested.
        QJSValue callback = dynamic_cast<JavaScriptCallback*>(reply->userData(0))->callback;
        QJSValue value = callback.engine()->toScriptValue(doc.object().value("GET").toVariant());
        callback.call(QJSValueList() << value);

        reply->deleteLater();
    }
    catch(std::exception& e)
    {
        if(reply == NULL)
            std::cerr << "[RedisInterface] handleGetRequestResponse_JavaScript(): Network reply is NULL!" << std::endl;
        else
            std::cerr << "[RedisInterface] handleGetRequestResponse_JavaScript(): Error: " << e.what() << std::endl;
    }
}

void RedisInterface::handleGetRequestResponse_MetaMethod()
{
    // Retrieve the network reply.
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        // Parse the JSON response.
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        // Find the MetaMethodCallback we tacked on earlier and call it with the value we requested.
        MetaMethodCallback* metaMethodCallback = dynamic_cast<MetaMethodCallback*>(reply->userData(0));
        QVariant value = doc.object().value("GET").toVariant();
        metaMethodCallback->callback.invoke(metaMethodCallback->requester, QGenericArgument("key", &metaMethodCallback->key), QGenericArgument("value", value.data()));

        reply->deleteLater();
    }
    catch(std::exception& e)
    {
        if(reply == NULL)
            std::cerr << "[RedisInterface] handleGetRequestResponse_MetaMethod(): Network reply is NULL!" << std::endl;
        else
            std::cerr << "[RedisInterface] handleGetRequestResponse_MetaMethod(): Error: " << e.what() << std::endl;
    }
}

/*!
 * \brief SETs the Redis property with the given \a{key} to the given \a{value}.
 */
void RedisInterface::set(QString key, const QVariant &value)
{
    QNetworkReply* setReply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + "SET/" + key + "/" + value.toString())));
    publish(QString(key + "_changed"), value);

    connect(setReply, SIGNAL(finished()), setReply, SLOT(deleteLater()));
}

/*!
 * \brief Performs a synchronous GET request for the Redis value with the given \a{key}. This method blocks the calling thread (which, in QML, is the main thread), therefore
 * it is highly recommended that you use one of the asynchronous overloads instead.
 */
QVariant RedisInterface::get(QString key) const
{
    // Perform the HTTP GET request for the key.
    QNetworkReply* reply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + "GET/" + key)));

    // Spin up an event loop and wait for the response.
    QEventLoop waitLoop;
    connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
    waitLoop.exec();

    // Parse the response and return the requested value to the caller.
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if(doc.isObject())
        return doc.object().value("GET").toVariant();
    else
        std::cerr << "[RedisInterface] Synchronous get(): Bad response from server!" << std::endl;

    return "";
}

/*!
 * \brief Performs an asynchronous GET request for the Redis value with the given \a{key}. When a response is received, the given JavaScript \a{callback}
 * will be invoked.
 */
void RedisInterface::get(QString key, QJSValue callback) const
{
    qDebug() << "[RedisInterface] Performing asynchronus GET request for" << key << "with callback" << callback.toString();

    // Perform the GET request.
    QNetworkReply* reply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + "GET/" + key)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(handleGetRequestResponse_JavaScript()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));

    // Tack the callback onto the reply as a UserData object. This will be unwrapped in handleGetRequestResponse_JavaScript().
    reply->setUserData(0, new JavaScriptCallback(callback));
}

/*!
 * \brief Performs an asynchronous GET request for the Redis value with the given \a{key}. When a response is received, the given C++ method \a{callback}
 * will be invoked.
 */
void RedisInterface::get(QString key, QMetaMethod callback) const
{
    qDebug() << "[RedisInterface] Performing asynchronus GET request for " << key << "with callback" << callback.name();

    // Perform the GET request.
    QNetworkReply* reply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + "GET/" + key)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(handleGetRequestResponse_MetaMethod()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));

    // Tack the callback onto the reply as a UserData object. this will be unwrapped in handleGetRequestResponse_MetaMethod().
    reply->setUserData(0, new MetaMethodCallback(parent(), key, callback));
}

/*!
 * \brief Performs a single-shot PUBLISH command via Redis.
 */
void RedisInterface::publish(QString remoteEventName, QVariant value)
{
//    qDebug() << "[RedisInterface] Publishing event" << remoteEventName << "via Redis, with value" << value.toString();
    QNetworkReply* reply = _networkInterface->get(QNetworkRequest(QUrl(_serverUrl + "PUBLISH/" + remoteEventName + "/" + value.toString())));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}
