import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import DroneControl

Window {
    width: 640
    height: 480
    visible: true
    title: "random Remote control"

    property var throttleStatus: [true, true, true, true]
    property alias debugMode: debugToggle.checked

    property int margins: 48

    UdpDrone {
        id: udpDrone
    }

    BtLEDrone {
        id: btleDrone
    }

    property var drone: udpDrone

    Button {
        visible: false
        onClicked: Qt.createQmlObject("import DroneControl; BtDrone {}", parent, "mfucklthis")
    }

    RowLayout {
        width: parent.width

        height: Math.max(heightCtrl.height, joystick.height)

        anchors.bottom: parent.bottom
        anchors.bottomMargin: margins

        Slider {
            id: heightCtrl

            property real lastValue: 0

            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.leftMargin: margins * 2

            orientation: Qt.Vertical

            from: -32768
            to: 32767

            onMoved: {5
                lastValue = value
            }

            onPressedChanged: {
                    // started pressing
                    if (pressed)
                        return

                    // basically onRelease:
                    lastValue = 0
                    value = 0
            }
        }

        Joystick {
            id: joystick

            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.rightMargin: margins * 2

            diameter: 100
        }
    }

    DebugControls {
        visible: debugMode
    }

    RowLayout {
        width: parent.width

        CheckBox {
            id: usebtle

            Layout.alignment: Qt.ALignLeft | Qt.AlignTop
            Layout.leftMargin: margins
            Layout.topMargin: margins

            text: "use BTLE"

            checked: drone === btleDrone
            onToggled: {
                drone.stop()

                if (drone === btleDrone)
                    drone = udpDrone
                else
                    drone = btleDrone

                drone.start()
            }

            Component.onCompleted: drone.start()
        }

        Item { Layout.fillWidth: true; } // spacer

        CheckBox {
            id: debugToggle
            text: "debug mode"

            Layout.alignment: Qt.ALignRight | Qt.AlignTop
            Layout.rightMargin: margins
            Layout.topMargin: margins
        }
    }
}
