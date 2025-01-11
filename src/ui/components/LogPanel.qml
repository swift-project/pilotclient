import QtQuick 2.15
import QtQuick.Layouts 2.15
import QtQuick.Controls 2.15

Rectangle {
    color: "black"
    width: parent.width
    height: parent.height * 0.4
    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            AvionicButton {
                labelText: "LOG"
                Layout.leftMargin: 10
                Layout.rightMargin: 2
                active: true
            }
            AvionicButton {
                labelText: "COM"
                Layout.leftMargin: 2
                Layout.rightMargin: 2
            }
            AvionicButton {
                labelText: "DLH123"
                Layout.leftMargin: 2
                Layout.rightMargin: 2
            }
            Item {
                Layout.fillWidth: true
            }
        }
        Rectangle {
            implicitWidth: parent.width - 20
            implicitHeight: 2
            color: "gray"
            Layout.leftMargin: 10
            Layout.rightMargin: 10
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 5

            width: parent.width * 0.4
            height: parent.height
            id: atcList
            color: "black"

            ScrollView {
                clip: true
                anchors.fill: parent


                ListView {
                    id: atcListView
                    anchors.fill: parent
                    boundsMovement: Flickable.StopAtBounds
                    model: TestModelLogs {}
                    verticalLayoutDirection: ListView.BottomToTop
                    property int selectedIndex: -1
                    delegate: Text {
                        id: label
                        color: "white"
                        font.family: "Mono" // TODO Use correct font
                        text: timestamp + ": " + log
                    }
                }
            }
        }

        Rectangle {
            height: 20
            Layout.fillWidth: true
            color: "#242424"
            border.color: chatInput.activeFocus ? "green" : "white"
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 0
            Layout.bottomMargin: 10

            TextInput {
                id: chatInput
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                anchors.fill: parent
                color: "white"
                font.family: "Mono" // TODO Use correct font
            }
        }


    }
}