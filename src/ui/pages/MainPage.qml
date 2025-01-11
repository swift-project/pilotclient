import QtQuick 2.15
import "../components"

Rectangle {
    color: "black"
    anchors.fill: parent

    ComPanel {
        id: comPanel
    }

    AtcPanel {
        id: atcPanel
        anchors.top: comPanel.bottom
        width: parent.width
        height: parent.height * 0.4
    }

    LogPanel {
        anchors.top: atcPanel.bottom
        id: logPanel
    }
}