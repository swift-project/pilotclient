/*
 * SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 * SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: levelMeter

    property double value: 0.97
    property double peak: 0.0

    width: 400
    height: 20

    onValueChanged: {
        if(value < 0.0) {
            value = 0.0
        } else if (value > 1.0) {
            value = 1.0;
        }
        if (value > peak) {
            peak = value;
            peakTimer.start()
        }
    }

    Row {
        anchors.fill: parent

        Rectangle {
            id: lowRangeDark
            width: levelMeter.width * 0.68
            height: parent.height
            color: "#002366"

            Rectangle {
                id: lowRange
                width: Math.min(lowRangeDark.width, value * levelMeter.width)
                height: parent.height
                color: "#4169E1"
            }
        }

        Rectangle {
            id: midRangeDark
            width: levelMeter.width * 0.22
            height: parent.height
            color: "#114911"

            Rectangle {
                id: midRange
                width: Math.min(midRangeDark.width, (value * levelMeter.width) - lowRangeDark.width)
                height: parent.height
                color: "#32CD32"
            }
        }

        Rectangle {
            id: highRangeDark
            width: levelMeter.width * 0.1
            height: parent.height
            color: "#760e0e"

            Rectangle {
                id: highRange
                width: Math.min(highRangeDark.width, (value  * levelMeter.width) - lowRangeDark.width - midRangeDark.width)
                height: parent.height
                color: "#FF4500"
            }
        }
    }

    Rectangle {
        id: peakBar
        width: 3
        height: parent.height
        color: "#3edd9f"
        x: peak * levelMeter.width
    }

    Timer {
        id: peakTimer
        onTriggered: peak = 0
    }
}
