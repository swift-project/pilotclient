// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERGRIDPROVIDER_H
#define BLACKMISC_WEATHER_WEATHERGRIDPROVIDER_H

#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/provider.h"
#include "blackmisc/slot.h"
#include "blackmisc/blackmiscexport.h"

#include <QObject>
#include <QtGlobal>

namespace BlackMisc
{
    class CIdentifier;

    namespace Weather
    {
        //! Direct threadsafe in memory access to weather grid
        class BLACKMISC_EXPORT IWeatherGridProvider
        {
        public:
            //! Copy constructor
            IWeatherGridProvider() = default;

            //! Copy constructor
            IWeatherGridProvider(const IWeatherGridProvider &) = delete;

            //! Copy assignment operator
            IWeatherGridProvider &operator=(const IWeatherGridProvider &) = delete;

            //! Destructor
            virtual ~IWeatherGridProvider() {}

            //! Request weather grid with identifier
            virtual void requestWeatherGrid(const BlackMisc::Geo::ICoordinateGeodetic &position, const BlackMisc::CIdentifier &identifier) = 0;

            //! Request weather grid
            //! \deprecated Use the position/identifier based version if possible. It will inject the result "in the simulator"
            virtual void requestWeatherGrid(const CWeatherGrid &weatherGrid, const CSlot<void(const CWeatherGrid &)> &callback) = 0;

            //! Request weather grid from file
            virtual void requestWeatherGridFromFile(const QString &filePath,
                                                    const BlackMisc::Weather::CWeatherGrid &weatherGrid,
                                                    const BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> &callback) = 0;
        };

        //! Delegating class which can be directly used to access an \sa IWeatherGridProvider instance
        class BLACKMISC_EXPORT CWeatherGridAware : public IProviderAware<IWeatherGridProvider>
        {
        public:
            //! \copydoc IWeatherGridProvider::requestWeatherGrid
            void requestWeatherGrid(const BlackMisc::Geo::ICoordinateGeodetic &position, const BlackMisc::CIdentifier &identifier);

            //! \copydoc IWeatherGridProvider::requestWeatherGrid
            //! \deprecated Use the position/identifier based version if possible. It will inject the result "in the simulator"
            void requestWeatherGrid(const CWeatherGrid &weatherGrid, const CSlot<void(const CWeatherGrid &)> &callback);

        protected:
            //! Constructor
            CWeatherGridAware(IWeatherGridProvider *weatherGridProvider) : IProviderAware(weatherGridProvider) { Q_ASSERT(weatherGridProvider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Weather::IWeatherGridProvider, "org.swift-project.blackmisc::weather::iweathergridprovider")

#endif // guard
