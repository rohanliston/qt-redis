TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    CppRedisTest.cpp \
    ../QRedisLib/RedisInterface.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    CppRedisTest.h \
    ../QRedisLib/RedisInterface.h
