// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_TEMPERATURELAYERLIST_H
#define BLACKMISC_WEATHER_TEMPERATURELAYERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/temperaturelayer.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CTemperatureLayer, CTemperatureLayerList)

namespace BlackMisc
{
    namespace Aviation
    {
        class CAltitude;
    }

    namespace Weather
    {
        /*!
         * Value object encapsulating a set of temperature layers
         */
        class BLACKMISC_EXPORT CTemperatureLayerList :
            public CSequence<CTemperatureLayer>,
            public BlackMisc::Mixin::MetaType<CTemperatureLayerList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CTemperatureLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CTemperatureLayerList() = default;

            //! Construct from a base class object.
            CTemperatureLayerList(const CSequence<CTemperatureLayer> &other);

            //! Contains temperature layer with level?
            bool containsLevel(const BlackMisc::Aviation::CAltitude &level) const;

            //! Find cloud layer by level
            CTemperatureLayer findByLevel(const BlackMisc::Aviation::CAltitude &level) const;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CTemperatureLayerList)

#endif // guard
