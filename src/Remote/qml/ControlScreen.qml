import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import DroneControl

Item {
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

            from: -256
            to: 255

            onMoved: {
                lastValue = value
            }

            onPressedChanged: {
                // started pressing
                if (pressed)
                    return

                // basically onRelease:
                lastValue = 0
                value = 0

                Backend.drone.forceClampThrottles();
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
        visible: Backend.debugMode
    }
}
