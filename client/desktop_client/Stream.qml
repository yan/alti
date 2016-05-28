import QtQuick 2.1
import QtQuick.Controls 1.0

Item {
    anchors.fill: parent

    Button {
        id: buttonStart
        x: 28
        y: 31
        text: qsTr("Start")
        onClicked: {
            console.log("device manager is: ", deviceManager)
            deviceManager.start()
        }
    }

    Button {
        id: buttonStop
        x: 30
        y: 63
        text: qsTr("Stop")
        onClicked: {
            console.log("device manager is: ", deviceManager)
            deviceManager.stop()
        }
    }
}
