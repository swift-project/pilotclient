import QtQuick 2.15
import QtQuick.Layouts 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: bottombar
    color: "#2f2f2f"

    RowLayout {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter

        StatusLight {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            labelText: "CONN"
            active: controller.component === "../pages/ConnectionPage.qml"
            onClick: controller.navigateToConnection()
        }
        StatusLight {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            labelText: "MAIN"
            active: controller.component === "../pages/MainPage.qml"
            onClick: controller.navigateToHome()
        }
        StatusLight {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            labelText: "CSTM"
        }
        StatusLight {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            labelText: "SETT"
        }
        StatusLight {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            labelText: "ADV"
            onClick: advancedMenu.popup()
        }

        Menu {
            id: advancedMenu
            MenuItem { text: "FPL" }
            MenuItem { text: "RADAR" }
            MenuItem { text: "SIM" }
        }
    }
}