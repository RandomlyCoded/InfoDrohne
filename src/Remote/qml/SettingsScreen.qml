import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import DroneControl

RowLayout {
    width: parent.width

    ColumnLayout {
        Layout.alignment: Qt.ALignLeft | Qt.AlignTop
        Layout.leftMargin: margins
        Layout.topMargin: margins

        RadioButton {
            text: "BtLE"

            checked: Backend.protocol === Backend.BtLE
            onToggled: Backend.protocol = Backend.BtLE
        }

        RadioButton {
            text: "Bt"

            checked: Backend.protocol === Backend.Bt
            onToggled: Backend.protocol = Backend.Bt
        }

        RadioButton {
            text: "UDP"

            checked: Backend.protocol === Backend.Udp
            onToggled: Backend.protocol = Backend.Udp
        }
    }

    Item { Layout.fillWidth: true; } // spacer

    ColumnLayout {
        Layout.alignment: Qt.ALignRight | Qt.AlignTop
        Layout.rightMargin: margins
        Layout.topMargin: margins

        Switch {
            id: debugToggle
            text: "debug mode"

            checked: Backend.debugMode

            onToggled: Backend.debugMode = checked

            visible: Backend.hasDebugMode();
            }

        Switch {
            id: loggingToggle
            text: "embedded logging"

            checked: Backend.enableEmbeddedLogging

            onToggled: Backend.enableEmbeddedLogging = checked
        }
    }
}
