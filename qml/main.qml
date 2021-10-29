
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import enroute

ApplicationWindow {
    id: window
    width: 700
    height: 700
    visible: true

    GridLayout {
        columns: 2
        anchors.fill: parent

        FlightMap {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true

            width: 700
            height: 700
        }

        Label {
            Layout.leftMargin: 5
            text: "Zoom"
        }
        Slider {
            Layout.fillWidth: true
            from: 7
            value: 10
            to: 14
        }
        Label {
            Layout.leftMargin: 5
            text: "Rotation"
        }
        Slider {
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
