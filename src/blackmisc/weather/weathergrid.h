// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERGRID_H
#define BLACKMISC_WEATHER_WEATHERGRID_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/weatherscenario.h"

#include <QMetaType>
#include <initializer_list>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CGridPoint, CWeatherGrid)

namespace BlackMisc
{
    namespace Geo
    {
        class ICoordinateGeodetic;
    }
    namespace Weather
    {
        /*!
         * Value object a list of weather grid points
         */
        class BLACKMISC_EXPORT CWeatherGrid :
            public CSequence<CGridPoint>,
            public Mixin::MetaType<CWeatherGrid>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CWeatherGrid)
            using CSequence::CSequence;

            //! Default constructor.
            CWeatherGrid() = default;

            //! Construct from a base class object.
            CWeatherGrid(const CSequence<CGridPoint> &other);

            //! Grid with an initial point at coordinates given
            CWeatherGrid(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

            //! \copydoc Geo::IGeoObjectList::findWithinRange
            CWeatherGrid findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! \copydoc Geo::IGeoObjectList::findClosest
            CWeatherGrid findClosest(int number, const BlackMisc::Geo::ICoordinateGeodetic &coordinate) const;

            //! Allow to describe myself
            QString getDescription(const QString &sep = "\n") const;

            //! Get all available weather scenarios
            static const QVector<CWeatherScenario> &getAllScenarios();

            //! Get weather grid by fixed scenario
            static const CWeatherGrid &getByScenario(const CWeatherScenario &scenario);

            //! Clear weather grid
            static const BlackMisc::Weather::CWeatherGrid &getClearWeatherGrid();

            //! Thunderstorm grid
            static const BlackMisc::Weather::CWeatherGrid &getThunderStormGrid();
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherGrid)

#endif // guard
