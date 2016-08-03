import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
// import QtQuick.Controls.Styles 1.4
import QtBluetooth 5.3

import org.srtd 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    function gotVal(pkt) {
        console.log(pkt)
        console.log(pkt.gps.coord)
    }
/*
    Connections {
        target: controller
        onDeviceFinished: {

            pageOne.groupBox1.enabled = true
            //pageOne.connected.
            console.log("Starting from QML")
            controller.gotSensors.connect(gotVal)
            controller.startStreamingSensors()

            //swipeView.children(1).label.text = device.name
        }
    }
*/
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page1 {
            id: pageOne

            connected.onClicked:   {
                if (connected.enabled && connected.checked) {
                    console.log("Switch pressed");
                    connected.enabled = false;
                    controller.tryConnect();
                    controller.deviceFinished.connect(function() {
                        connected.enabled = true;
                        console.log("Found? ");
                    });
                } else {
                    console.log("Disconnecting");
                    controller.tryDisconnect();
                }
            }

            btnStart.onClicked: {
                console.log("Starting..");
                controller.startRecordingEvent();
            }

            btnStop.onClicked: {
                console.log("Stopping..");
                controller.stopRecordingEvent();
            }

            btnFormat.onClicked: {
                console.log("Formatting..");
                controller.formatStorage();
            }

            btnListEvents.onClicked: {
                console.log("Listing events..")
                controller.getEvents();
            }

        }

        Page {
            id: pageTwo
            Label {
                id: label
                text: qsTr("Second page")
                anchors.centerIn: parent
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("First")
        }
        TabButton {
            text: qsTr("Second")
        }
    }
}
