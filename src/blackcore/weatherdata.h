/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEATHERDATA_H
#define BLACKCORE_WEATHERDATA_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/weather/weathergrid.h"
#include <QObject>

namespace BlackCore
{
    /*!
     * Interface to weather data
     */
    class BLACKCORE_EXPORT IWeatherData : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~IWeatherData() {}

        //! Fetch new weather data around grid
        virtual void fetchWeatherData(const BlackMisc::Weather::CWeatherGrid &grid,
                                      const BlackMisc::PhysicalQuantities::CLength &range) = 0;

        //! Get fetched weather data
        virtual BlackMisc::Weather::CWeatherGrid getWeatherData() const = 0;

    signals:
        //! Finished fetching data
        void fetchingFinished();

    protected:
        //! Default constructor
        IWeatherData(QObject *parent = nullptr) : QObject(parent) {}
    };

    /*!
     * Factory pattern class to create instances of IWeatherData
     */
    class BLACKCORE_EXPORT IWeatherDataFactory
    {
    public:
        //! Virtual destructor
        virtual ~IWeatherDataFactory() {}

        //! Create a new instance
        virtual IWeatherData *create(QObject *parent = nullptr) = 0;
    };
} // namespace

Q_DECLARE_INTERFACE(BlackCore::IWeatherDataFactory, "org.swift-project.blackcore.weatherdata")

#endif // guard
