import QtQuick 2.7
import QtQuick.Controls 1.1

Rectangle {
    property alias btnPublishEvent1: btnPublishEvent1
    property alias btnPublishEvent2: btnPublishEvent2
    property alias btnGetAsync: btnGetAsync
    property alias btnGetSync: btnGetSync
    property alias btnSetRemoteDouble: btnSetRemoteDouble
    property alias btnSetRemoteString: btnSetRemoteString
    property alias btnClearAll: btnClearAll

    Grid {
        spacing: 10
        columns: 2

        property real buttonWidth: (parent.width / 2)

        Button {
            id: btnPublishEvent1
            width: parent.buttonWidth
            text: "PUBLISH remote:subscribed:event1"
        }

        Button {
            id: btnPublishEvent2
            width: parent.buttonWidth
            text: "PUBLISH remote:subscribed:event2"
        }

        Button {
            id: btnGetAsync
            width: parent.buttonWidth
            text: "Asynchronous GET get:async"
        }

        Button {
            id: btnGetSync
            width: parent.buttonWidth
            text: "Synchronous GET get:sync"
        }

        Button {
            id: btnSetRemoteDouble
            width: parent.buttonWidth
            text: "SET remote:published:double"
        }

        Button {
            id: btnSetRemoteString
            width: parent.buttonWidth
            text: "SET remote:published:string"
        }

        Button {
            id: btnClearAll
            width: parent.buttonWidth
            text: "Clear all"
        }
    }
}
