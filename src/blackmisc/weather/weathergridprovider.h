/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERGRIDPROVIDER_H
#define BLACKMISC_WEATHER_WEATHERGRIDPROVIDER_H

#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/provider.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/slot.h"

#include <QObject>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Weather
    {
        //! Direct threadsafe in memory access to weather grid
        class BLACKMISC_EXPORT IWeatherGridProvider
        {
        public:
            //! Request weather grid
            virtual void requestWeatherGrid(const CWeatherGrid &weatherGrid,
                                            const CSlot<void(const CWeatherGrid &)> &callback) = 0;
        };

        //! Delegating class which can be directly used to access an \sa IWeatherGridProvider instance
        class BLACKMISC_EXPORT CWeatherGridAware : public IProviderAware<IWeatherGridProvider>
        {
        public:
            //! \copydoc IWeatherGridProvider::requestWeatherGrid
            virtual void requestWeatherGrid(const CWeatherGrid &weatherGrid,
                                            const CSlot<void(const CWeatherGrid &)> &callback);

        protected:
            //! Constructor
            CWeatherGridAware(IWeatherGridProvider *weatherGridProvider) : IProviderAware(weatherGridProvider) { Q_ASSERT(weatherGridProvider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Weather::IWeatherGridProvider, "org.swift-project.blackmisc::weather::iweathergridprovider")

#endif // guard
