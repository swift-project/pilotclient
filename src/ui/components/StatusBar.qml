import QtQuick 2.15
import QtQuick.Layouts 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: topbar
    color: "#2f2f2f"

    RowLayout {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter

        StatusLight {
            Layout.margins: 10
            labelText: "S"
            disabledColor: "#323232"

        }
        StatusLight {
            labelText: "SIM"
            disabledColor: "#323232"
            Layout.margins: 10
        }
        StatusLight {
            labelText: "NET"
            disabledColor: "#323232"
            active: network.connectionStatus
            Layout.margins: 10
        }
        StatusLight {
            labelText: "PTT"
            disabledColor: "#323232"
            Layout.margins: 10
        }
        StatusLight {
            labelText: "STS"
            disabledColor: "#323232"
            Layout.margins: 10
        }


        Text {
            id: callsign
            text: "-----"
            color: "white"
            font.bold: true
            Layout.margins: 10

        }

        Item {
            Layout.fillWidth: true
        }


        Rectangle {
            Layout.alignment: Qt.AlignRight
            Layout.margins: 10
            Layout.preferredWidth: 25
            Layout.preferredHeight: 25
            color: "red"
            Text {
                anchors.centerIn: parent
                text: "X"
                font.pointSize: 16
            }
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }
    }
}