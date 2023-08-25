// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_WEATHERMANAGER_H
#define BLACKCORE_WEATHERMANAGER_H

#include "blackcore/pluginmanagerweatherdata.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/weather/weathergridprovider.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifier.h"
#include "blackmisc/slot.h"

#include <QObject>
#include <QVector>

namespace BlackCore
{
    class IWeatherData;

    /*!
     * CWeatherManager
     */
    class BLACKCORE_EXPORT CWeatherManager :
        public QObject,
        public BlackMisc::Weather::IWeatherGridProvider
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Weather::IWeatherGridProvider)

    public:
        //! Default constructor
        CWeatherManager(QObject *parent = nullptr);

        //! Override weather to clear
        void setWeatherToClear(bool value);

        //! Is weather overwritten to clear?
        bool isWeatherClear() const { return m_isWeatherClear; }

        //! \copydoc BlackMisc::Weather::IWeatherGridProvider::requestWeatherGrid
        virtual void requestWeatherGrid(const BlackMisc::Geo::ICoordinateGeodetic &position, const BlackMisc::CIdentifier &identifier) override;

        //! \copydoc BlackMisc::Weather::IWeatherGridProvider::requestWeatherGrid
        virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &initialWeatherGrid,
                                        const BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> &callback) override;

        //! \copydoc BlackMisc::Weather::IWeatherGridProvider::requestWeatherGrid
        virtual void requestWeatherGridFromFile(const QString &filePath,
                                                const BlackMisc::Weather::CWeatherGrid &initialWeatherGrid,
                                                const BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> &callback) override;

    signals:
        //! The weather grid, requested from identified, is available
        void weatherGridReceived(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier);

    private:
        //! Pending weather request
        struct WeatherRequest
        {
            QString filePath;
            BlackMisc::CIdentifier identifier;
            BlackMisc::Weather::CWeatherGrid weatherGrid;
            BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> callback;
        };

        void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &initialWeatherGrid, const BlackMisc::CIdentifier &identifier);
        bool loadWeatherDataPlugins();
        void fetchNextWeatherData();
        void fetchNextWeatherDataDeferred();
        void appendRequest(const WeatherRequest &request);
        void handleNextRequest();

        CPluginManagerWeatherData m_pluginManagerWeatherData { this };
        QVector<IWeatherData *> m_weatherDataPlugins;
        QVector<WeatherRequest> m_pendingRequests;
        qint64 m_lastPendingRequestTs = -1;
        BlackMisc::Weather::CWeatherGrid m_weatherGrid;
        bool m_isWeatherClear = false;
    };
} // ns

#endif // guard
