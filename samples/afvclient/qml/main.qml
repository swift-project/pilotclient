import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

ApplicationWindow {
    id: window
    width: 1200
    height: 520
    visible: true
    title: "Audio For Vatsim"

    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
    }

    Grid {
        id: leftGrid
        columns: 2
        rows: 6
        spacing: 10
        padding: 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.leftMargin: 10

        Label {
            id: lblUsername
            width: 100
            text: qsTr("Username")
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: false
        }

        TextField {
            id: tfUsername
            width: 350
            height: 25
            text: qsTr("1234567")
            selectByMouse: true
            horizontalAlignment: Text.AlignLeft
            renderType: Text.NativeRendering
        }

        Label {
            id: lblPassword
            width: 100
            text: qsTr("Password")
            Layout.fillWidth: false
            Layout.fillHeight: false
            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: tfPassword
            width: 350
            height: 25
            text: qsTr("123456")
            selectByMouse: true
            echoMode: TextInput.PasswordEchoOnEdit
            horizontalAlignment: Text.AlignLeft
            renderType: Text.NativeRendering
        }

        Label {
            id: lblCallsign
            width: 100
            text: qsTr("Callsign")
            Layout.fillWidth: false
            Layout.fillHeight: false
            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: tfCallsign
            width: 350
            height: 25
            text: qsTr("DECHK")
            selectByMouse: true
            horizontalAlignment: Text.AlignLeft
            renderType: Text.NativeRendering
        }

        Label {
            id: lblInputDevice
            width: 100
            text: qsTr("Input Device")
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: false
        }

        ComboBox {
            id: cbInputDevices
            width: 350
            height: 25
            model: voiceClient.availableInputDevices()
        }

        Label {
            id: lblOutputDevice
            width: 100
            text: qsTr("Output Device")
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: false
        }

        ComboBox {
            id: cbOutputDevices
            width: 350
            height: 25
            model: voiceClient.availableOutputDevices()
        }

        Frame {
            background: Rectangle {
                color: "transparent"
                border.color: "transparent"
            }
        }

        Row {
            spacing: 10

            Button {
                id: btConnect

                property bool connected: false
                width: 170
                height: 25
                text: qsTr("Connect")
                onClicked: {
                    if (btConnect.connected) {
                        btConnect.connected = false;
                        btConnect.text = qsTr("Connect")
                        voiceClient.disconnectFrom()
                    } else {
                        btConnect.connected = true
                        btConnect.text = qsTr("Disconnect")
                        voiceClient.connectTo(tfUsername.text, tfPassword.text, tfCallsign.text)
                        afvMapReader.setOwnCallsign(tfCallsign.text)
                    }

                }
            }

            Button {
                id: btStartAudio

                property bool started: false
                width: 170
                height: 25
                text: qsTr("Start Audio")
                onClicked: {
                    btStartAudio.enabled = false
                    cbInputDevices.enabled = false
                    cbOutputDevices.enabled = false
                    voiceClient.start(cbInputDevices.currentText, cbOutputDevices.currentText)
                }
            }
        }
    }

    Grid {
        id: rightGrid
        padding: 10
        anchors.top: parent.top
        anchors.left: leftGrid.right
        anchors.right: parent.right
        spacing: 10
        rows: 2
        columns: 3

        Transceiver { id: transceiver1; transceiverId: 0 }

        SpinBox {
            id: sbAltitude
            width: 150
            height: 40
            stepSize: 500
            to: 50000
            from: 0
            value: 1000
        }

        Label {
            id: lblReceivingCom1
            height: 40
            text: qsTr("Receiving:")
            verticalAlignment: Text.AlignVCenter
        }

        Transceiver { id: transceiver2; transceiverId: 1 }

        Button {
            id: btUpdateStack
            width: 150
            height: 40
            text: qsTr("Update Stack")
            onClicked: {
                voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
                voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
                voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value)
                voiceClient.updateTransceivers()
            }
        }

        Label {
            id: lblReceivingCom2
            height: 40
            text: qsTr("Receiving:")
            verticalAlignment: Text.AlignVCenter
            // anchors.verticalCenter: parent.verticalCenter
        }
    }

    Column {
        id: column
        padding: 10
        spacing: 10
        anchors.top: rightGrid.bottom
        anchors.left: leftGrid.right
        anchors.right: parent.right

        ProgressBar {
            id: pbAudioInput
            width: 500
            height: 25
            anchors.left: parent.left
            anchors.leftMargin: 10
            value: voiceClient.inputVolumePeakVU
        }

        ProgressBar {
            id: pbAudioOutput
            width: 500
            height: 25
            anchors.left: parent.left
            anchors.leftMargin: 10
            value: voiceClient.outputVolumePeakVU
        }
    }

////    CheckBox {
////        id: cbVhfEffects
////        anchors.topMargin: 5
////        anchors.leftMargin: 10
////        anchors.left: sbAltitude.right
////        anchors.top: parent.top
////        anchors.verticalCenter: sbAltitude.verticalCenter
////        height: 25
////        text: qsTr("VHF Effects")
////        checked: true
////    }

    Map {
        id: map
        anchors.topMargin: 5
        anchors.top: leftGrid.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        plugin: mapPlugin
        center: QtPositioning.coordinate(48.50, 11.50) // Oslo
        zoomLevel: 3

//        MapCircle {
//            center {
//                latitude: map.center.latitude
//                longitude: map.center.longitude
//            }
//            radius: 500000.0
//            color: 'blue'
//            border.width: 3
//            border.color: 'blue'
//            opacity: 0.3
//        }

        MapItemView {
            model: afvMapReader.atcStationModel
            delegate: atcDelegate
        }

        Component {
            id: atcDelegate
            AtcRing {
                position {
                    latitude: latitude
                    longitude: longitude
                }
                radius: radioDistanceM
                cs: callsign
                freqAsString: frequencyAsString
                freqKhz: frequencyKhz
                onSelected: {
                    map.center = QtPositioning.coordinate(latitude, longitude)
                    transceiver1.frequency = frequency
                    voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
                    voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
                    voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value)

                }
            }
        }

        Rectangle {
            width: 3
            height: 15
            color: "blue"
            anchors.verticalCenter: map.verticalCenter
            anchors.horizontalCenter: map.horizontalCenter
        }

        Rectangle {
            width: 15
            height: 3
            color: "blue"
            anchors.verticalCenter: map.verticalCenter
            anchors.horizontalCenter: map.horizontalCenter
        }
    }

    Timer {
        interval: 5000; running: true; repeat: true
        onTriggered: {
            voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
            voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
            voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value)
        }
    }
}
