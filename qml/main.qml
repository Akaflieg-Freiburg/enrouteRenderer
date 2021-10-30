
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

        GestureEvent

        FlightMap {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true

            bearing: bearingSlider.value
            zoom: zoomSlider.value

            width: 700
            height: 700

            MouseArea {
            }
        }

        Label {
            Layout.leftMargin: 5
            text: "Zoom"
        }
        Slider {
            id: zoomSlider

            Layout.fillWidth: true
            from: 7
            value: 10
            to: 14
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
