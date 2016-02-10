import QtQuick 2.5
import QtQuick.Controls 1.4


Rectangle {

    width: 360
    height: 360
    property alias buttonStop: buttonStop

    property alias buttonStart: buttonStart

    Button {
        id: buttonStart
        x: 8
        y: 8
        text: qsTr("Start")
    }

    Button {
        id: buttonStop
        x: 10
        y: 40
        text: qsTr("Stop")
    }
}

