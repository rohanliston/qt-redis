import QtQuick 2.2
import QtQuick.Controls 1.1
import MyComponents.QtRedis

RedisInterface {
    id: root
    serverUrl: "http://localhost:7379/"
    width: 800
    height: 600

    subscribedProperties: [
        { remote: "remote:published:double", local: "localSubscribedDouble" },
        { remote: "remote:published:string", local: "localSubscribedString" }
    ]

    publishedProperties: [
        { local: "localPublishedDouble", remote: "remote:subscribed:double" },
        { local: "localPublishedString", remote: "remote:subscribed:string" }
    ]

    subscribedEvents: [
        { remote: "remote:signal:trigger", local: "localSignal"   },
        { remote: "remote:method:trigger", local: "localMethod"   },
        { remote: "remote:wildcard*",      local: "localWildcard" }
    ]

    publishedEvents: [
        { local: "publishedSignal1", remote: "remote:subscribed:event1" },
        { local: "publishedSignal2", remote: "remote:subscribed:event2" }
    ]

    Component.onCompleted: init();
    property double localSubscribedDouble
    property string localSubscribedString
    property double localPublishedDouble: 0.00
    property string localPublishedString: "."
    property string asynchronousGetVariable: ""
    property string synchronousGetVariable: ""

    signal publishedSignal1
    signal publishedSignal2
    signal localSignal
    onLocalSignal: handleLocalSignal();

    function localMethod() {
        console.log("(QML) localMethod() called");
    }

    function localWildcard() {
        console.log("(QML) localWildcard() called");
    }

    function handleLocalSignal() {
        console.log("(QML) localSignal() emitted and handled by handleLocalSignal()");
    }

    Text {
        anchors.centerIn: parent
        text: "remote:published:double : "   + localSubscribedDouble.toFixed(20) + "\n"   +
              "remote:published:string : \"" + localSubscribedString             + "\"\n" +
              "localPublishedDouble    : "   + localPublishedDouble.toFixed(2)   + "\n"   +
              "localPublishedString    : \"" + localPublishedString              + "\"\n" +
              "get:async               : \"" + asynchronousGetVariable           + "\"\n" +
              "get:sync                : \"" + synchronousGetVariable            + "\""
    }

    Grid {
        spacing: 10
        columns: 2

        property real buttonWidth: (parent.width / 2)

        Button {
            width: parent.buttonWidth
            text: "PUBLISH remote:subscribed:event1"
            onClicked: publishedSignal1()
        }

        Button {
            width: parent.buttonWidth
            text: "PUBLISH remote:subscribed:event2"
            onClicked: publishedSignal2()
        }

        Button {
            width: parent.buttonWidth
            text: "Asynchronous GET get:async"
            onClicked: {
                get("get:async", function(value){
                    asynchronousGetVariable = value;
                });
            }
        }

        Button {
            width: parent.buttonWidth
            text: "Synchronous GET get:sync"
            onClicked: {
                synchronousGetVariable = get("get:sync");
            }
        }

        Button {
            width: parent.buttonWidth
            text: "SET remote:published:double"
            onClicked: {
                set("remote:published:double", Math.random());
            }
        }

        Button {
            width: parent.buttonWidth
            text: "SET remote:published:string"
            onClicked: {
                set("remote:published:string", Math.random().toString(36).substring(7));
            }
        }

        Button {
            width: parent.buttonWidth
            text: "Clear all"
            onClicked: {
                localPublishedDouble = 0.0;
                localPublishedString = ".";

                localSubscribedDouble = 0.0;
                localSubscribedString = "";
                synchronousGetVariable = "";
                asynchronousGetVariable = "";
            }
        }
    }

    Timer {
        running: true
        repeat: true
        interval: 16
        onTriggered: {
            localPublishedDouble += 0.1;
            localPublishedString += ".";

            if(localPublishedString.length > 10)
                localPublishedString = ".";
        }
    }
}
