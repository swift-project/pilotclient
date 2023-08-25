// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKWXPLUGIN_GFS_H
#define BLACKWXPLUGIN_GFS_H

#include "g2clib/grib2.h"
#include "blackmisc/network/url.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/worker.h"
#include "blackcore/weatherdata.h"
#include <QReadWriteLock>
#include <QHash>
#include <QVector>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPointer>
#include <array>

namespace BlackMisc::PhysicalQuantities
{
    class CTemperature;
}
namespace BlackWxPlugin::Gfs
{
    struct Grib2ParameterKey;
    struct Grib2ParameterValue;
    struct GfsGridPoint;

    /*!
     * GFS implemenation
     */
    class CWeatherDataGfs : public BlackCore::IWeatherData
    {
        Q_OBJECT

    public:
        //! Constructor
        CWeatherDataGfs(QObject *parent = nullptr);

        //! Destructor
        virtual ~CWeatherDataGfs() override;

        //! \copydoc BlackCore::IWeatherData::fetchWeatherData
        virtual void fetchWeatherData(const BlackMisc::Weather::CWeatherGrid &initialGrid,
                                      const BlackMisc::PhysicalQuantities::CLength &range) override;

        //! \copydoc BlackCore::IWeatherData::fetchWeatherDataFromFile
        virtual void fetchWeatherDataFromFile(const QString &filePath,
                                              const BlackMisc::Weather::CWeatherGrid &grid,
                                              const BlackMisc::PhysicalQuantities::CLength &range) override;

        //! \copydoc BlackCore::IWeatherData::getWeatherData()
        virtual BlackMisc::Weather::CWeatherGrid getWeatherData() const override;

    private:
        //! Asyncronous fetching finished
        //! \threadsafe
        void fetchingWeatherDataFinished();

        void parseGfsFile(QNetworkReply *nwReplyPtr);
        BlackMisc::Network::CUrl getDownloadUrl() const;
        bool parseGfsFileImpl(const QByteArray &gribData);
        void findNextGribMessage(unsigned char *buffer, g2int size, g2int iseek, g2int *lskip, g2int *lgrib);
        void createWeatherGrid(const gribfield *gfld);
        void handleProductDefinitionTemplate40(const gribfield *gfld);
        void handleProductDefinitionTemplate48(const gribfield *gfld);
        void setTemperature(const g2float *fld, float level);
        void setHumidity(const g2float *fld, float level);
        void setWindV(const g2float *fld, float level);
        void setWindU(const g2float *fld, float level);
        void setCloudCoverage(const g2float *fld, int level);
        void setCloudLevel(const g2float *fld, int surfaceType, int level);
        void setCloudTemperature(const g2float *fld, int surfaceType, int level);
        void setPressureAtMsl(const g2float *fld);
        void setSurfaceRain(const g2float *fld);
        void setSurfaceSnow(const g2float *fld);
        void setPrecipitationRate(const g2float *fld);

        BlackMisc::PhysicalQuantities::CTemperature calculateDewPoint(const BlackMisc::PhysicalQuantities::CTemperature &temperature, double relativeHumidity);

        BlackMisc::Weather::CWeatherGrid m_grid;
        BlackMisc::PhysicalQuantities::CLength m_maxRange;

        mutable QReadWriteLock m_lockData;
        QByteArray m_gribData;

        QVector<GfsGridPoint> m_gfsWeatherGrid;
        BlackMisc::Weather::CWeatherGrid m_weatherGrid;

        QPointer<BlackMisc::CWorker> m_parseGribFileWorker; //!< worker will destroy itself, so weak pointer

        using Grib2ParameterKey = std::array<g2int, 2>;
        using Grib2ParameterTable = QMap<Grib2ParameterKey, Grib2ParameterValue>;
        static const Grib2ParameterTable m_grib2ParameterTable;
    };

    //! Factory for creating CWeatherDataGfs instance
    class CWeatherDataGfsFactory : public QObject, public BlackCore::IWeatherDataFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.weatherdata" FILE "weatherdatagfs.json")
        Q_INTERFACES(BlackCore::IWeatherDataFactory)

    public:
        //! \copydoc BlackCore::IWeatherDataFactory::create()
        virtual BlackCore::IWeatherData *create(QObject *parent = nullptr) override;
    };

} // ns

#endif // guard
