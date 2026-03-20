import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import DroneControl

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("random Remote control")

    flags: Backend.windowFlags()

    property var throttleStatus: [true, true, true, true]

    property int margins: 48

    TabBar {
        id: tabs

        width: parent.width

        TabButton {
            text: qsTr("controls")
        }

        TabButton {
            text: qsTr("settings")
        }
    }

    StackLayout {
        anchors.fill: parent
        anchors.topMargin: tabs.height

        currentIndex: tabs.currentIndex

        ControlScreen {}
        SettingsScreen {}
    }
}
