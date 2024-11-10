/*
 * SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 * SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

Rectangle {
    id: window
    width: 1200
    height: 800
    visible: true
    // title: "Audio For VATSIM"

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        /**
                anchors.topMargin: 5
                anchors.top: leftGrid.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                **/
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
                    // voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
                    // voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
                    voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value )
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

        SpinBox {
            id: sbAltitude
            width: 150
            height: 40
            stepSize: 500
            to: 50000
            from: 0
            value: 1000
        }
    }

    Timer {
        interval: 5000; running: true; repeat: true
        onTriggered: {
            // voiceClient.updateComFrequency(0, transceiver1.frequency * 1000)
            // voiceClient.updateComFrequency(1, transceiver2.frequency * 1000)
            voiceClient.updatePosition(map.center.latitude, map.center.longitude, sbAltitude.value )
        }
    }
}
