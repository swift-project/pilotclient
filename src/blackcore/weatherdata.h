// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

        //! Fetch new weather around grid from file
        virtual void fetchWeatherDataFromFile(const QString &filePath,
                                              const BlackMisc::Weather::CWeatherGrid &grid,
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
