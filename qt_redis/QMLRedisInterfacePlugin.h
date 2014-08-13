#ifndef FILEDOWNLOADER_PLUGIN_H
#define FILEDOWNLOADER_PLUGIN_H

#include <QQmlExtensionPlugin>

class QMLRedisInterfacePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MyComponents.QtRedis")

public:
    void registerTypes(const char *uri);
};

#endif // FILEDOWNLOADER_PLUGIN_H
