import QtQuick
import QtQuick.Controls

import DroneControl

Rectangle {
    anchors.fill: parent

    color: "gray"
    opacity: 0.9

    MouseArea {
        // there is probably a better way to achieve this, but idrc rn
        anchors.fill: parent
        enabled: parent.visible

        onClicked:       (mouse) => { mouse.accepted = true; }
        onDoubleClicked: (mouse) => { mouse.accepted = true; }
        onPressAndHold:  (mouse) => { mouse.accepted = true; }
        onPressed:       (mouse) => { mouse.accepted = true; }
        onReleased:      (mouse) => { mouse.accepted = true; }
        onWheel:         (wheel) => { wheel.accepted = true; }
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
            model: Backend.drone.throttles

            Column {
                spacing: 4

                Text {
                    width: slider.width
                    text: Math.round(slider.value/32767 * 100) + "%"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Slider {
                    id: slider
                    orientation: Qt.Vertical

                    value: modelData
                    from: 0
                    to: 32767

                    enabled: enabledToggle.checked

                    onMoved: Backend.drone.setSingleThrottle(model.index, value)
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: indexInfo

                    text: model.index
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                CheckBox {
                    id: enabledToggle
                    checked: throttleStatus[model.index]

                    onToggled: throttleStatus[model.index] = checked
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Column {
            spacing: 4

            Text {
                width: sliderModifier.width

                text: Math.round(sliderModifier.value/32767 * 100) + "%"

                anchors.horizontalCenter: parent.horizontalCenter
            }

            Slider {
                id: sliderModifier
                property real lastValue: 0

                orientation: Qt.Vertical

                from: -32768
                to: 32767

                onMoved: {
                    var newThrottles = [0, 0, 0, 0]
                    let dt = value - lastValue;

                    for (var i = 0; i < Backend.drone.propCount; ++i) {
                        if (throttleStatus[i])
                            newThrottles[i] = Backend.drone.throttles[i] + dt

                        else
                            newThrottles[i] = Backend.drone.throttles[i]
                    }

                    Backend.drone.throttles = newThrottles

                    lastValue = value
                }

                onPressedChanged: {
                        // started pressing
                        if (pressed)
                            return

                        // basically onRelease:

                        // we let the throttles go outside their normal (u16) range, so we need to reset them
                        Backend.drone.forceClampThrottles()

                        lastValue = 0
                        value = 0
                }
            }
        }
    }
}
