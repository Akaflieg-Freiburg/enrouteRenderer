import QtPositioning
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import enroute

ApplicationWindow {
    id: window
    width: 800
    height: 800
    visible: true

    GridLayout {
        columns: 2
        anchors.fill: parent

        FlightMap {
            id: flightMap

            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true

            bearing: bearingSlider.value

            width: 700
            height: 700

            MouseArea {
                anchors.fill: parent

                property var centerOnClicked

                onPositionChanged: function (mouse) {
                    flightMap.setCenter(centerOnClicked, Qt.point(mouse.x,mouse.y))
                }
                onPressed: function (mouse) {
                    centerOnClicked = flightMap.fromPixelCoordinate(Qt.point(mouse.x,mouse.y))
                }

                onWheel: function (wheel) {
                    flightMap.setZoom(flightMap.zoom*Math.exp(0.0001*wheel.angleDelta.y), Qt.point(wheel.x,wheel.y))

                }
            }
        }

        Label {
            Layout.leftMargin: 5
            text: "Bearing"
        }
        Slider {
            id: bearingSlider

            Layout.fillWidth: true
            from: 0
            value: 0
            to: 360
        }
    }


    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: Qt.quit()
    }

}
