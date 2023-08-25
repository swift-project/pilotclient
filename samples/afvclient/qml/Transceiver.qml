/*
 * SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 * SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Row {
    id: idTransceiver
    property int transceiverId: 0
    property alias frequency: sbFrequency.value
    property alias rxOn: cbEnabled.checked
    property alias txOn: cbTxOn.checked

    spacing: 10
    Label {
        id: lblRadio
        text: 'Radio ' + transceiverId
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    SpinBox {
        id: sbFrequency
        width: 150
        height: 40
        editable: true
        stepSize: 25
        to: 140000
        from: 110000
        value: 122800

        property int decimals: 3
        property real realValue: value / 1000

        validator: DoubleValidator {
            bottom: Math.min(sbFrequency.from, sbFrequency.to)
            top:  Math.max(sbFrequency.from, sbFrequency.to)
        }

        textFromValue: function(value, locale) {
            return Number(value / 1000).toLocaleString(locale, 'f', sbFrequency.decimals)
        }

        valueFromText: function(text, locale) {
            return Number.fromLocaleString(locale, text) * 1000
        }
        wheelEnabled: true
    }

    CheckBox {
        id: cbTxOn
        height: 25
        text: qsTr("TX")
        checked: true
        anchors.verticalCenter: parent.verticalCenter
        onClicked: idTransceiver.txOnChanged(checked)
    }

    CheckBox {
        id: cbEnabled
        height: 25
        text: qsTr("RX")
        checked: true
        anchors.verticalCenter: parent.verticalCenter
        onClicked: idTransceiver.rxOnChanged(checked)
    }
}
