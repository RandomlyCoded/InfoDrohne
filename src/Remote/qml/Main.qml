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

    UdpDrone {
        id: udpDrone
    }

    BtLEDrone {
        id: btleDrone
    }

    property var drone: udpDrone

    CheckBox {
        id: usebtle

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

    Button {
        visible: false
        onClicked: Qt.createQmlObject("import DroneControl; BtDrone {}", parent, "mfucklthis")
    }

    RowLayout {
        width: parent.width

        height: Math.max(heightCtrl.height, joystick.height)

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24

        Slider {
            id: heightCtrl

            property real lastValue: 0

            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.leftMargin: 24

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
            Layout.rightMargin: 24

            diameter: 100
        }
    }

    DebugControls {
        visible: debugMode
    }

    CheckBox {
        id: debugToggle
        text: "debug mode"
        anchors.right: parent.right
    }
}
