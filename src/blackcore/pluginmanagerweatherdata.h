/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_PLUGINMANAGERWEATHERDATA_H
#define BLACKCORE_PLUGINMANAGERWEATHERDATA_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/pluginmanager.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/weatherdataplugininfo.h"
#include "blackmisc/weather/weatherdataplugininfolist.h"

#include <QHash>
#include <QMap>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    class CVariant;
}
namespace BlackCore
{
    class IWeatherDataFactory;

    /*!
     * Manages plugins of type WeatherData.
     */
    class BLACKCORE_EXPORT CPluginManagerWeatherData :
        public BlackCore::IPluginManager
    {
        Q_OBJECT

    public:
        //! Ctor
        CPluginManagerWeatherData(QObject *parent = nullptr);

        //! Get weatherdata factory from the plugin
        IWeatherDataFactory *getFactory(const QString &pluginId);

        //! Get all weather data plugins
        BlackMisc::Weather::CWeatherDataPluginInfoList getAvailableWeatherDataPlugins() const;

        //! \copydoc BlackCore::IPluginManager::collectPlugins()
        virtual void collectPlugins() override;

    protected:
        //! \copydoc BlackCore::IPluginManager::acceptedIids()
        virtual BlackMisc::CSequence<QString> acceptedIids() const override;

        //! \copydoc BlackCore::IPluginManager::pluginDirectory()
        virtual const QString &pluginDirectory() const override;

    private:
        //! Extended data for plugin
        struct PluginExtended
        {
            BlackMisc::Weather::CWeatherDataPluginInfo info;
        };

        QMap<QString, PluginExtended> m_plugins; //!< Id <-> extended data pairs
    };
} // namespace

#endif // guard
