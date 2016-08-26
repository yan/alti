import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtCanvas3D 1.1
import Qt.labs.folderlistmodel 2.1
//import QtDataVisualization 1.2
//import QtQuick.Scene3D 2.0
//import QtQuick.Templates 2.0
//import Qt3D.Core 2.0
//import Qt3D.Render 2.0

Item {
    id: item1
    ColumnLayout {
        id: columnLayout1
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        RowLayout {
            transformOrigin: Item.Top
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Switch {
                id: connected
                x: 0
                y: 56
                text: qsTr("Connected")
            }

            BusyIndicator {
                id: busy
                //x: 152
                //y: 62
                visible: false
                enabled: false
                // Layout.fillHeight: true
                width: 32
                height: 32
                Layout.maximumHeight: 32
                Layout.maximumWidth: 32
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        GroupBox {
            id: grpLogger
            width: 200
            height: 200
            transformOrigin: Item.Top
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            enabled: false
            title: qsTr("Logging")

            GridLayout {
                id: gridLayout
                rows: 2
                columns: 2
                anchors.fill: parent
                Button {
                    id: btnFormat
                    text: qsTr("Format")
                }

                Button {
                    id: btnListEvents
                    text: qsTr("List Events")
                }

                Button {
                    id: btnStart
                    text: qsTr("Start")
                }

                Button {
                    id: btnStop
                    text: qsTr("Stop")
                }

                Button {
                    id: btnSelectMostRecent
                    text: qsTr("Select Last Event")
                }

                Button {
                    id: btnRetrieveMostRecent
                    text: qsTr("Dump Event")
                }


            }


        }

        GroupBox {
            id: grpSensors
            width: 200
            height: 200
            transformOrigin: Item.Top
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            enabled: true
            title: qsTr("Sensors")

            ColumnLayout {
                id: sensorsLayout

                RowLayout {
                    id: rowLayout2
                    anchors.fill: parent
                    Button {
                        id: btnStartSensors
                        text: qsTr("Connect Sensors")
                    }

                    Button {
                        id: btnStopSensors
                        text: qsTr("Disconnect Sensors")
                    }
                }
                /*
                Canvas3D {
                    id: canvas
                    Layout.fillWidth: true
                    height: 100
                    property double xRot: 0.0
                    property double yRot: 45.0
                }*/
            }
        }

    }





    property alias btnStart: btnStart
    property alias btnStop: btnStop
    property alias grpLogger: grpLogger
    property alias connected: connected
    property alias busy: busy
    property alias btnFormat: btnFormat
    property alias btnListEvents: btnListEvents
    property alias btnSelectMostRecent: btnSelectMostRecent
    property alias btnRetrieveMostRecent: btnRetrieveMostRecent
    // property alias groupBox1: groupBox1

    /*
    Surface3D {
        x: 141
        y: 84
        width: 300
        height: 300
        Surface3DSeries {
            ItemModelSurfaceDataProxy {
                itemModel: ListModel {
                    ListElement {
                        y: "1"
                        row: "1"
                        column: "1"
                    }

                    ListElement {
                        y: "2"
                        row: "1"
                        column: "2"
                    }

                    ListElement {
                        y: "3"
                        row: "2"
                        column: "1"
                    }

                    ListElement {
                        y: "4"
                        row: "2"
                        column: "2"
                    }
                }
                rowRole: "row"
                columnRole: "column"
                yPosRole: "y"
            }
        }
    }
    */
}
