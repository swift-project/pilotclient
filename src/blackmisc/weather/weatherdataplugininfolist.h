// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERDATAPLUGININFOLIST_H
#define BLACKMISC_WEATHER_WEATHERDATAPLUGININFOLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/weatherdataplugininfo.h"

#include <QMetaType>
#include <QStringList>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CWeatherDataPluginInfo, CWeatherDataPluginInfoList)

namespace BlackMisc::Weather
{
    class CWeatherDataPluginInfo;

    //! Value object encapsulating a list of CWeatherDataPluginInfo objects.
    class BLACKMISC_EXPORT CWeatherDataPluginInfoList :
        public BlackMisc::CSequence<CWeatherDataPluginInfo>,
        public BlackMisc::Mixin::MetaType<CWeatherDataPluginInfoList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CWeatherDataPluginInfoList)
        using CSequence::CSequence;

        //! Default constructor
        CWeatherDataPluginInfoList();

        //! Construct from a base class object.
        CWeatherDataPluginInfoList(const CSequence<CWeatherDataPluginInfo> &other);

        //! String list with meaningful representations
        QStringList toStringList(bool i18n = false) const;
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherDataPluginInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Weather::CWeatherDataPluginInfo>)

#endif // guard
