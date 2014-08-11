#include "QRedisLib_plugin.h"
#include "QMLRedisInterface.h"
#include <qqml.h>

void QRedisLibPlugin::registerTypes(const char *uri)
{
    // @uri MyQMLComponents.Redis
    qmlRegisterType<QMLRedisInterface>(uri, 1, 0, "RedisInterface");
}


