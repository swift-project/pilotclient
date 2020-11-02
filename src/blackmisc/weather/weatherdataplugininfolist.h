/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERDATAPLUGININFOLIST_H
#define BLACKMISC_WEATHER_WEATHERDATAPLUGININFOLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/weatherdataplugininfo.h"

#include <QMetaType>
#include <QStringList>

namespace BlackMisc
{
    namespace Weather
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
} // ns

Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherDataPluginInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Weather::CWeatherDataPluginInfo>)

#endif // guard
