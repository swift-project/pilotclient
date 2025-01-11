import QtQuick 2.15


Rectangle {
    width: text.text.length === 1 ? text.width * 2.5 : text.width * 1.2;
    height: 30
    property string labelText: ""
    property string disabledColor: "white"

    property bool active: false

    signal click()

    color: "black"
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: parent.click()
    }
    Text {
        id: text
        anchors.margins: 20
        anchors.centerIn: parent
        text: labelText
        color: active ? "#26ff00" : disabledColor
        font.pointSize: 12
        font.family: "Mono" // TODO Use correct font
    }
}
