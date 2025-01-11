import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import "../components"


Rectangle {
    anchors.fill: parent
    color: "#323232"

    ColumnLayout {
        anchors.fill: parent
        Text {
            Layout.alignment: Qt.AlignCenter

            id: simselecttext

            text: "SELECT YOUR SIMULATOR"
            color: "white"
            font.family: "Mono" // TODO Use correct font
        }

        Repeater {
            model: simulator.availableSimulators
            SimulatorButton {
                required property string modelData
                labelText: modelData
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    simulator.setPluginIdentifier(modelData, false)
                }
            }
        }
    }

}