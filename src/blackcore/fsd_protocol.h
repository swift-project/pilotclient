//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef FSD_PROTOCOL_H
#define FSD_PROTOCOL_H

#include <QString>

namespace FSD
{
    enum SimulatorType {
        SIM_UNKNOWN = -1,
    };

    typedef enum {
        Query_FP,
        Query_Frequency,
        Query_Server,
        Query_RealName,
        Query_isATC,
        Query_Capabilities,
        Query_IP
    } TQueryType;

    typedef enum {
        TMode_Standby   = 'S',
        TMode_Charly    = 'N',
        TMode_Ident     = 'Y'
    } TTransponderMode;

    typedef struct {
        TTransponderMode    transponderMode;
        quint16             squawk;
        quint8              rating;
        double              latitude;
        double              longitude;
        qint32              altitude;
        qint32              groundSpeed;
        double              pitch;
        double              bank;
        double              heading;
        qint32              diffPressureTrueAlt;
    } TPositionMessage;


    const QString Headers[] = {
        "@",
        "%",
        "#AA",
        "#AP",
        "#DA",
        "#DP",
        "#TM"
    };

#define     MAX_FSD_HEADERS     7
}

#endif // FSD_PROTOCOL_H
