import QtQuick

import QtQuick.Controls

import DroneControl

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Drone {
        id: drone
    }

    Row {
        spacing: 20
        anchors.centerIn: parent
        CheckBox {
            text: "auto stabilize"
            checked: true
        }

        Column {
            spacing: 8
            Button {
                text: "up"
            }
            Button {
                text: "down"
            }
            Button {
                text: "rotate clockwise"
            }
            Button {
                text: "forwards"
            }
        }

        Repeater {
            model: drone.propCount

            Slider {
                orientation: Qt.Vertical

                Text {
                    text: model.index

                    anchors.top: parent.bottom
                    anchors.margins: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

        }
    }
}
