/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_SERIALIZER_H
#define BLACKCORE_FSD_SERIALIZER_H

#include "enums.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/network/facilitytype.h"

#include <QtGlobal>
#include <QString>

namespace BlackCore::Fsd
{
    //! \cond
    template<typename T>
    inline QString toQString(const T& value);

    template<typename T>
    T fromQString(const QString &str);

    template<>
    QString toQString(const AtcRating &value);

    template<>
    AtcRating fromQString(const QString &str);

    template<>
    QString toQString(const PilotRating &value);

    template<>
    PilotRating fromQString(const QString &str);

    template<>
    QString toQString(const SimType &value);

    template<>
    SimType fromQString(const QString &str);

    template<>
    QString toQString(const BlackMisc::Network::CFacilityType &value);

    template<>
    BlackMisc::Network::CFacilityType fromQString(const QString &str);

    template<>
    QString toQString(const ClientQueryType &value);

    template<>
    ClientQueryType fromQString(const QString &str);

    template<>
    QString toQString(const FlightType &value);

    template<>
    FlightType fromQString(const QString &str);

    template<>
    QString toQString(const BlackMisc::Aviation::CTransponder::TransponderMode &value);

    template<>
    BlackMisc::Aviation::CTransponder::TransponderMode fromQString(const QString &str);

    template<>
    QString toQString(const Capabilities& value);

    template<>
    Capabilities fromQString(const QString &str);

    template<>
    AtisLineType fromQString(const QString &str);
    //! \endcond
}

#endif // guard
