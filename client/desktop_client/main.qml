import QtQuick 2.5
import QtQuick.Window 2.2


Window {
    visible: true

    MainForm {
        anchors.fill: parent
        buttonStop.onClicked: {
            console.log("manager is: ", deviceManager);
            deviceManager.stop();
        }
        buttonStart.onClicked: {
            console.log("device manager is: ", deviceManager)
            deviceManager.start()
        }


    }
}

