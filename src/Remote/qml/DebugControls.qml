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
                text: "set all to 0"
                onClicked: {
                    console.log("resetting all motors")
                    Backend.drone.throttles = [0, 0, 0, 0]
                }
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
            model: Backend.drone.propCount

            Column {
                spacing: 4

                Text {
                    width: slider.width
                    text: Math.round(slider.value/slider.to * 100) + "%"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Slider {
                    id: slider
                    orientation: Qt.Vertical

                    stepSize: 1

                    value: Backend.drone.throttles[modelData]
                    from: 0
                    to: 255

                    enabled: enabledToggle.checked
                    live: true
                    wheelEnabled: true

                    onMoved: Backend.drone.setSingleThrottle(modelData, value)
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: indexInfo

                    text: modelData
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                CheckBox {
                    id: enabledToggle
                    checked: throttleStatus[modelData]

                    onToggled: throttleStatus[modelData] = checked
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Button {
                    text: "+"

                    width: slider.width

                    visible: slider.enabled

                    onClicked: {
                        slider.increase()
                        slider.moved()
                    }
                }

                Button {
                    text: "-"

                    width: slider.width

                    visible: slider.enabled

                    onClicked: {
                        slider.decrease()
                        slider.moved()
                    }
                }
            }
        }

        Column {
            spacing: 4

            Text {
                width: sliderModifier.width

                text: Math.round(sliderModifier.value/sliderModifier.to * 100) + "%"

                anchors.horizontalCenter: parent.horizontalCenter
            }

            Slider {
                id: sliderModifier
                property real lastValue: 0

                orientation: Qt.Vertical

                from: -256
                to: 255

                stepSize: 1

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
