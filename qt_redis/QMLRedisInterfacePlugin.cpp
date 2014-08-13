#include "QMLRedisInterfacePlugin.h"
#include "QMLRedisInterface.h"

#include <qqml.h>

void QMLRedisInterfacePlugin::registerTypes(const char *uri)
{
    // @uri MyComponents.QtRedis
    qmlRegisterType<QMLRedisInterface>(uri, 1, 0, "RedisInterface");
}
