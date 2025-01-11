import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    id: root
    width: 700
    height: 500
    title: "swift"

    SimSelection {
        anchors.fill: parent
        visible: !simulator.simulatorPluginSelected
    }

    MainLayout {
        anchors.fill: parent
        visible: simulator.simulatorPluginSelected

    }

}