import QtQuick

import QtQuick.Controls

import DroneControl

Window {
    width: 640
    height: 480
    visible: true
    title: "random Remote control"

    property var throttleStatus: [true, true, true, true]

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
                onPressed: console.log("start flying up (increase all throttles?)")
                onReleased: console.log("stop flying up (reset all throttles again)")
            }

            Button {
                text: "down"
                onPressed: console.log("start flying down (decrease all throttles?)")
                onReleased: console.log("stop flying down (reset all throttles again)")
            }

            Button {
                text: "rotate clockwise"

                onPressed: console.log("start rotating")
                onReleased: console.log("stop rotating")
            }

            Button {
                text: "forwards"

                onPressed: console.log("start flying forwards (increase back throttle slightly?)")
                onReleased: console.log("stop flying forwars (reset all throttles again)")
            }
        }

        Repeater {
            model: drone.throttles

            Slider {
                orientation: Qt.Vertical

                value: modelData
                from: 0
                to: 32767

                onMoved: drone.setSingleThrottle(model.index, value)

                Text {
                    id: indexInfo

                    text: model.index

                    anchors.top: parent.bottom
                    anchors.margins: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: Math.round(parent.value/32767 * 100) + "%"

                    anchors.bottom: parent.top
                    anchors.margins: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                CheckBox {
                    id: enableToggle
                    checked: throttleStatus[model.index]

                    onToggled: throttleStatus[model.index] = checked

                    anchors.top: indexInfo.bottom
                    anchors.margins: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                MouseArea {
                    anchors.fill: parent
                    property bool enabled: !enableToggle.checked

                    onPressed: (event) => { event.accepted = enabled }
                }
            }
        }

        Slider {
            property real lastValue: 0

            orientation: Qt.Vertical

            from: -32768
            to: 32767

            onMoved: {
                var newThrottles = [0, 0, 0, 0]
                let dt = value - lastValue;

                for (var i = 0; i < drone.propCount; ++i) {
                    if (throttleStatus[i])
                        newThrottles[i] = drone.throttles[i] + dt

                    else
                        newThrottles[i] = drone.throttles[i]
                }

                drone.throttles = newThrottles

                lastValue = value
            }

            onPressedChanged: {
                    // started pressing
                    if (pressed)
                        return

                    // basically onRelease:

                    // we let the throttles go outside their normal (u16) range, so we need to reset them
                    drone.forceClampThrottles()

                    lastValue = 0
                    value = 0
            }

            Text {
                text: Math.round(parent.value/32767 * 100) + "%"

                anchors.bottom: parent.top
                anchors.margins: 4
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
