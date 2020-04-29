import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import MyComponents.QtRedis 1.0

ApplicationWindow {
    width: 800
    height: 600
    visible: true
    title: qsTr("Redis Example")

    RedisContainer {
        id: root
        anchors.fill: parent
        serverUrl: "http://localhost:7379/"
    }
}

