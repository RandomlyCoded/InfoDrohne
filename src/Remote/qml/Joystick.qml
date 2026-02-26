import QtQuick

Rectangle {
    property real diameter: 50

    radius: diameter/2
    width: diameter
    height: diameter

    color: "#d2d2d2"

    MouseArea {
        anchors.fill: parent
        onReleased: handle.moveTo(parent.width/2, parent.height/2)

        onPositionChanged: (event) => handle.moveTo(event.x, event.y)
    }

    Rectangle {
        id: handle

        function moveTo(ax, ay) {
            // clamp
            ax -= parent.width/2
            ay -= parent.height/2

            let lensqr = ax * ax + ay * ay
            if (lensqr > parent.radius * parent.radius) {
                let len = Math.sqrt(lensqr)
                ax = ax/len * parent.radius
                ay = ay/len * parent.radius
            }

            x = ax + parent.width/2  - width/2
            y = ay + parent.height/2 - height/2

            drone.direction = Qt.vector3d(ax - parent.width/2, ay - parent.height/2, 0)
        }

        radius: diameter/20
        width: radius * 2
        height: radius * 2

        x: parent.width/2 - width/2
        y: parent.height/2 - height/2

        color: "black"
    }
}
