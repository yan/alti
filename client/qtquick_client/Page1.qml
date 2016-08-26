import QtQuick 2.7

Page1Form {

    connected.onEnabledChanged: {
        busy.visible = ! connected.enabled;
        busy.enabled = ! connected.enabled;
        grpLogger.enabled = connected.enabled;
    }
/*
    btnStop.onClicked: {
        console.log("Button 2 clicked.");
    }
    */
}
