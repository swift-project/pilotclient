import QtQuick 2.15
import "../components"
import "../pages"
import QtQuick.Layouts 2.15

ColumnLayout {
    visible: false
    spacing: 0
    anchors.fill: parent
    StatusBar {
        Layout.preferredWidth: parent.width
        Layout.preferredHeight: 50
        id: topbar
    }
    Rectangle {
        id: main
        Layout.preferredWidth: parent.width
        Layout.preferredHeight: parent.height - topbar.Layout.preferredHeight - bottombar.Layout.preferredHeight
        color: "#000000"
        Loader {
            id: dataArea
            anchors.fill: parent
            source: controller.component
        }
    }
    NavigationBar {
        id: bottombar
        Layout.fillWidth: true
        Layout.preferredHeight: 50
    }
}