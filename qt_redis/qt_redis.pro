message("Don't forget to run MAKE INSTALL with this module.");
message("You'll need to set the PREFIX environment variable to the path you want the libraries to be placed in.");

QT       -= gui
QT       += quick

TARGET = QtRedis
TEMPLATE = lib

DESTDIR =
# Comment this out if you wish to see PropertyTransport debug output.
message("Comment out the QT_NO_DEBUG_OUTPUT line in qt_redis.pro if you wish to see debug output.")
DEFINES += QT_NO_DEBUG_OUTPUT

# Don't comment out this line, however - it disables debug output for release builds.
CONFIG(release):DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += \
    RedisInterface.cpp \
    QMLRedisInterface.cpp \
    QMLRedisInterfacePlugin.cpp

HEADERS += \
    RedisInterface.h \
    QMLRedisInterface.h \
    QMLRedisInterfacePlugin.h

headers.path = $$(PREFIX)/include/qt_redis
headers.files += $$HEADERS
target.path = $$(PREFIX)/lib

# Generates qmldir file
uri = MyComponents.QtRedis
INSTALL_PATH = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
plugin_dump.path = $${INSTALL_PATH}
plugin_dump.commands = echo module MyComponents.QtRedis > $${INSTALL_PATH}/qmldir && echo plugin $$TARGET $$target.path >> $${INSTALL_PATH}/qmldir

INSTALLS += target headers plugin_dump

OTHER_FILES +=

# --- Extra make target to build documentation.
docs.target = docs
docs.commands = qdoc $$PWD/doc/qtredis.qdocconf
QMAKE_EXTRA_TARGETS += docs
