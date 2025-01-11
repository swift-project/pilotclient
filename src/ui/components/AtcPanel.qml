import QtQuick 2.15
import QtQuick.Layouts 2.15
import QtQuick.Controls 2.15

Rectangle {
    color: "black"

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            AvionicButton {
                Layout.leftMargin: 10
                Layout.rightMargin: 2

                labelText: "CTR"
                active: true
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "APP"
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "TWR"
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "GND"
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "DEL"
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "OTHER"
            }
            AvionicButton {
                Layout.leftMargin: 2
                Layout.rightMargin: 2
                labelText: "ATIS"
            }
            Item {
                Layout.fillWidth: true
            }
        }
        Rectangle {
            implicitWidth: parent.width - 20
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            height: 2
            color: "gray"
        }
        Rectangle {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 10
            Layout.fillHeight: true
            Layout.fillWidth: true


            Rectangle {
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
                        model: TestModel {}

                        property int selectedIndex: -1
                        delegate: Rectangle {
                            width: atcListView.width
                            height: label.implicitHeight
                            color: index === atcListView.selectedIndex ? "green" : "black"
                            Text {
                                id: label
                                color: "white"
                                font.family: "Mono" // TODO Use correct font
                                text: name + " - " + frequency
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: atcListView.selectedIndex = index
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: atisView
                border.color: "gray"
                anchors.left: atcList.right
                width: parent.width * 0.6
                height: parent.height
                color: "black"

                Text {
                    anchors.margins: 5
                    anchors.fill: parent
                    text: "Joe Doe\nCallsign BREMEN GROUND\nAirport Briefing vats.im/edww | NO PDC/DCL\nSubmit your feedback at feedback.vatger.de"
                    color: "white"
                    font.family: "Mono" // TODO Use correct font
                }
            }
        }

    }
}