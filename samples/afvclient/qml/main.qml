/*
 * SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 * SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "Audio For Vatsim"

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Grid {
        id: leftGrid
        anchors.bottom: column.bottom
        anchors.bottomMargin: 0
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

        property alias mapUsername: tfUsername.text
        TextField {
            id: tfUsername
            objectName: "loginUsername"
            width: 350
            height: 25
            text: userName
            selectByMouse: true
            enabled: voiceClient.connectionStatus == 0 // Disconnected
            horizontalAlignment: Text.AlignLeft
            renderType: Text.NativeRendering
            // text: qsTr("1234567")
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
            enabled: voiceClient.connectionStatus == 0 // Disconnected
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
            enabled: voiceClient.connectionStatus == 0 // Disconnected
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
                width: 170
                height: 25
                text: voiceClient.connectionStatus == 0 ? "Connect" : "Disconnect"
                onClicked: {
                    if (voiceClient.connectionStatus == 0) {
                        voiceClient.connectTo(tfUsername.text, tfPassword.text, tfCallsign.text)
                        afvMapReader.setOwnCallsign(tfCallsign.text)
                    } else if (voiceClient.connectionStatus == 1) {
                        voiceClient.disconnectFrom()
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
                    voiceClient.startAudio(cbInputDevices.currentText, cbOutputDevices.currentText)
                }
            }
        }
    }

    Grid {
        id: rightGrid
        padding: 0
        anchors.top: parent.top
        anchors.left: leftGrid.right
        anchors.right: parent.right
        anchors.topMargin: 10
        spacing: 10
        rows: 3
        columns: 3

        Transceiver {
            id: transceiver1
            transceiverId: 0
            onRxOnChanged: {
                voiceClient.enableTransceiver(transceiverId, rxOn)
            }
        }

        SpinBox {
            id: sbAltitude
            width: 150
            height: 40
            stepSize: 500
            to: 50000
            from: 0
            value: 1000
            wheelEnabled: true
        }

        Label {
            id: lblReceivingCom1
            height: 40
            text: qsTr("Receiving: ") + voiceClient.receivingCallsignsCom1
            verticalAlignment: Text.AlignVCenter
        }

        Transceiver {
            id: transceiver2
            transceiverId: 1
            txOn: false
            onRxOnChanged: {
                voiceClient.enableTransceiver(transceiverId, rxOn)
            }
        }

        Button {
            id: btUpdateStack
            width: 150
            height: 40
            text: qsTr("Update Stack")
            onClicked: {
                voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
                voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
                voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value)
            }
        }

        Label {
            id: lblReceivingCom2
            height: 40
            text: qsTr("Receiving: ") + voiceClient.receivingCallsignsCom2
            verticalAlignment: Text.AlignVCenter
            // anchors.verticalCenter: parent.verticalCenter
        }
    }

    Column {
        id: column
        spacing: 5
        anchors.top: rightGrid.bottom
        anchors.left: leftGrid.right
        anchors.right: parent.right

        Row {
            id: row1
            anchors.topMargin: 0
            padding: 0
            spacing: 10

            LevelMeter {
                id: pbAudioInput
                width: 400
                height: 15
                value: voiceClient.inputVolumePeakVU
                anchors.verticalCenter: parent.verticalCenter
            }

            Slider {
                id: slInputVolume
                from: -18
                to: 18
                value: 0
                anchors.verticalCenter: parent.verticalCenter
                wheelEnabled: true
                onMoved: voiceClient.setInputVolumeDb(value)
            }
        }

        Row {
            id: row2
            anchors.topMargin: 0
            padding: 0
            spacing: 10

            LevelMeter {
                id: pbAudioOutput
                width: 400
                height: 15
                value: voiceClient.outputVolumePeakVU
                anchors.verticalCenter: parent.verticalCenter
            }

            Slider {
                id: slOutputVolume
                from: 0
                to: 100
                value: 50
                anchors.verticalCenter: parent.verticalCenter
                wheelEnabled: true
                onMoved: voiceClient.setNormalizedMasterOutputVolume(value)
            }
        }

        Row {
            id: row3
            padding: 0
            spacing: 10

            CheckBox {
                id: cbVhfEffects
                text: qsTr("VHF Effects")
                checked: true
                anchors.verticalCenter: parent.verticalCenter
                onClicked: voiceClient.setBypassEffects(!checked)
            }

            CheckBox {
                id: cbLoopback
                text: qsTr("Loopback")
                checked: false
                anchors.verticalCenter: parent.verticalCenter
                onClicked: voiceClient.setLoopBack(checked)
            }

            Button {
                id: btPtt
                width: 150
                height: 40
                text: qsTr("PTT")
                onPressed: voiceClient.setPtt(true)
                onReleased: voiceClient.setPtt(false)
                background: Rectangle {
                          implicitWidth: btPtt.width
                          implicitHeight: btPtt.height
                          color: btPtt.down ? "lightgreen" : "lightgrey"
                          border.width: 1
                          radius: 2
                      }
            }

            Label {
                function translateStatus(status) {
                    switch(status) {
                    case 0: return "Disconnected"
                    case 1: return "Connected"
                    default: return "Unknown"
                    }
                }

                id: lblStatus
                text: "Status: " + translateStatus(voiceClient.connectionStatus)
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Map {
        id: map
        anchors.topMargin: 5
        anchors.top: column.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        plugin: mapPlugin
        center: QtPositioning.coordinate(48.50, 11.50) // Oslo
        zoomLevel: 3

        MapItemView {
            id: mapItemView
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

        Button {
            id: btZoomIn
            width: 30
            height: 30
            text: "+"
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: btZoomOut.top
            anchors.bottomMargin: 5
            onClicked: map.zoomLevel = map.zoomLevel + 1
        }

        Button {
            id: btZoomOut
            width: 30
            height: 30
            text: "-"
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            onClicked: map.zoomLevel = map.zoomLevel - 1
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
