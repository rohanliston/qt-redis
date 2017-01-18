import QtQuick 2.7
import QtQuick.Window 2.2
import Redis 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    MainForm {
        id: form
        anchors.fill: parent

        btnPublishEvent1.onClicked: redis.publishedSignal1()
        btnPublishEvent2.onClicked: redis.publishedSignal2()
        btnGetAsync.onClicked: {
            redis.get("get:async", function(value){
                redis.asynchronousGetVariable = value;
            })
        }
        btnGetSync.onClicked: {
            redis.synchronousGetVariable = redis.get("get:sync");
        }
        btnSetRemoteDouble.onClicked: {
            redis.set("remote:published:double", Math.random());
        }
        btnSetRemoteString.onClicked: {
            redis.set("remote:published:string", Math.random().toString(36).substring(7));
        }
        btnClearAll.onClicked: {
            redis.localPublishedDouble = 0.0;
            redis.localPublishedString = ".";

            redis.localSubscribedDouble = 0.0;
            redis.localSubscribedString = "";
            redis.synchronousGetVariable = "";
            redis.asynchronousGetVariable = "";
        }
    }

    RedisInterface {
        id: redis
        serverUrl: "http://localhost:7379/"
        width: 800
        height: 600

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

        Component.onCompleted: init();

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

        Text {
            anchors.centerIn: parent
            text: "remote:published:double : "   + redis.localSubscribedDouble.toFixed(20) + "\n"   +
                  "remote:published:string : \"" + redis.localSubscribedString             + "\"\n" +
                  "localPublishedDouble    : "   + redis.localPublishedDouble.toFixed(2)   + "\n"   +
                  "localPublishedString    : \"" + redis.localPublishedString              + "\"\n" +
                  "get:async               : \"" + redis.asynchronousGetVariable           + "\"\n" +
                  "get:sync                : \"" + redis.synchronousGetVariable            + "\""
        }

        Timer {
            running: true
            repeat: true
            interval: 16
            onTriggered: {
                redis.localPublishedDouble += 0.1;
                redis.localPublishedString += ".";

                if(redis.localPublishedString.length > 10)
                    redis.localPublishedString = ".";
            }
        }
    }
}
