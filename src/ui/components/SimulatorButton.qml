import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    property string labelText: ""

    id: control
    implicitWidth: parent.width * 0.5
    font.family: "Mono" // TODO Use correct font
    contentItem: Label {
        text: labelText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "white"

    }
    background: Rectangle {
        implicitWidth: parent.width * 0.5
        implicitHeight: 20
        color: control.down ? "#111111" : "#000000"
    }
}