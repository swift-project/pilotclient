import QtQuick 2.15
import QtQuick.Layouts 2.15

Rectangle {
    width: parent.width
    height: parent.height * 0.2
    color: "black"


    RowLayout {
        anchors.fill: parent
        Text {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            text: "COM1"
            color: "white"
            font.family: "Mono" // TODO Use correct font
            font.pointSize: 20

        }

        Text {
            text: "122.800"
            color: "white"
            font.family: "Mono" // TODO Use correct font
            font.pointSize: 20
        }

        ColumnLayout {
            Text {
                text: "TX"
                color: "white"
                font.family: "Mono" // TODO Use correct font
            }
            Text {
                text: "RX"
                color: "white"
                font.family: "Mono" // TODO Use correct font
            }
        }

        Item {
            Layout.fillWidth: true
        }


        Text {
            text: "COM2"
            color: "white"
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            font.family: "Mono" // TODO Use correct font
            font.pointSize: 20
        }

        Text {
            text: "121.500"
            color: "white"
            font.family: "Mono" // TODO Use correct font
            font.pointSize: 20
        }

        ColumnLayout {
            Layout.rightMargin: 10
            Text {
                text: "TX"
                color: "white"
                font.family: "Mono" // TODO Use correct font
            }
            Text {
                text: "RX"
                color: "white"
                font.family: "Mono" // TODO Use correct font
            }
        }

    }

}