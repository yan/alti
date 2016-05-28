import QtQuick 2.5
import QtQuick.Controls 1.4


Rectangle {

    width: 360
    height: 360
    property alias receive1: receive1


    TabView {
        id: tabView1
        anchors.fill: parent

        Tab {
            id: stream1
            title: "Stream"
            source: "Stream.qml"
        }

        Tab {
            id: receive1
            title: "Receive"
            source: "Receive.qml"

            TableView {
                id: tableView1
                x: 152
                y: 42
                width: 200
                height: 292
                anchors.top: tabView1.bottom
                anchors.topMargin: -318
                anchors.bottom: tabView1.top
                anchors.bottomMargin: -334
                anchors.left: tabView1.right
                anchors.leftMargin: -208
                anchors.right: tabView1.left
                anchors.rightMargin: -352
                anchors.horizontalCenter: tabView1.horizontalCenter
                anchors.verticalCenter: tabView1.verticalCenter
            }
        }
    }
}

