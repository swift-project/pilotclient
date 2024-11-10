// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_SERIALIZER_H
#define SWIFT_CORE_FSD_SERIALIZER_H

#include "enums.h"
#include "misc/aviation/transponder.h"
#include "misc/network/facilitytype.h"

#include <QtGlobal>
#include <QString>

namespace swift::core::fsd
{
    //! \cond
    template <typename T>
    inline QString toQString(const T &value);

    template <typename T>
    T fromQString(const QString &str);

    template <>
    QString toQString(const AtcRating &value);

    template <>
    AtcRating fromQString(const QString &str);

    template <>
    QString toQString(const PilotRating &value);

    template <>
    PilotRating fromQString(const QString &str);

    template <>
    QString toQString(const SimType &value);

    template <>
    SimType fromQString(const QString &str);

    template <>
    QString toQString(const swift::misc::network::CFacilityType &value);

    template <>
    swift::misc::network::CFacilityType fromQString(const QString &str);

    template <>
    QString toQString(const ClientQueryType &value);

    template <>
    ClientQueryType fromQString(const QString &str);

    template <>
    QString toQString(const FlightType &value);

    template <>
    FlightType fromQString(const QString &str);

    template <>
    QString toQString(const swift::misc::aviation::CTransponder::TransponderMode &value);

    template <>
    swift::misc::aviation::CTransponder::TransponderMode fromQString(const QString &str);

    template <>
    QString toQString(const Capabilities &value);

    template <>
    Capabilities fromQString(const QString &str);

    template <>
    AtisLineType fromQString(const QString &str);
    //! \endcond
}

#endif // guard
