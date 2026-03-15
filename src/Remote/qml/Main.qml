import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import DroneControl

Window {
    width: 640
    height: 480
    visible: true
    title: "random Remote control"

    flags: Backend.windowFlags()

    property var throttleStatus: [true, true, true, true]

    property int margins: 48

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

    RowLayout {
        width: parent.width

        ColumnLayout {
            Layout.alignment: Qt.ALignLeft | Qt.AlignTop
            Layout.leftMargin: margins
            Layout.topMargin: margins

            CheckBox {
                text: "BtLE"

                checked: Backend.protocol === Backend.BtLE
                onToggled: Backend.protocol = Backend.BtLE
            }

            CheckBox {
                text: "Bt"

                checked: Backend.protocol === Backend.Bt
                onToggled: Backend.protocol = Backend.Bt
            }

            CheckBox {
                text: "UDP"

                checked: Backend.protocol === Backend.Udp
                onToggled: Backend.protocol = Backend.Udp
            }
        }

        Item { Layout.fillWidth: true; } // spacer

        CheckBox {
            id: debugToggle
            text: "debug mode"

            onToggled: Backend.toggleDebug();

            visible: Backend.hasDebugMode();

            Layout.alignment: Qt.ALignRight | Qt.AlignTop
            Layout.rightMargin: margins
            Layout.topMargin: margins
        }
    }
}
