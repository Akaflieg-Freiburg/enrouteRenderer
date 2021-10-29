
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 700
    height: 700
    visible: true

    GridLayout {
        columns: 2
        anchors.fill: parent

        Rectangle {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true

            width: 700
            height: 700
            color: "blue"
        }

        Label {
            text: "Zoom"
        }
        Slider {
            Layout.fillWidth: true
            from: 7
            value: 10
            to: 14
        }
        Label {
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
