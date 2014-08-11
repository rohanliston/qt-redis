#ifndef QREDISLIB_PLUGIN_H
#define QREDISLIB_PLUGIN_H

#include <QQmlExtensionPlugin>

class QRedisLibPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // QREDISLIB_PLUGIN_H

