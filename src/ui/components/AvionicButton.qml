import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    property string labelText: ""
    property bool active: false

    id: control
    implicitWidth: contentLabel.implicitWidth * 1.1
    implicitHeight: contentLabel.implicitHeight * 1.1
    font.family: "Mono" // TODO Use correct font
    contentItem: Label {
        id: contentLabel
        text: labelText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: active ? "#00ff00" : "#ffffff"

    }
    background: Rectangle {
        implicitWidth: contentLabel.implicitWidth
        implicitHeight: contentLabel.implicitHeight
        color: "#555555"
    }
}