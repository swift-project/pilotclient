/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_PLUGIN_MANAGER_SIMULATOR_H
#define BLACKCORE_PLUGIN_MANAGER_SIMULATOR_H

#include "blackcoreexport.h"
#include "pluginmanager.h"
#include "blackmisc/pluginstorageprovider.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include <QObject>

namespace BlackCore
{
    class ISimulatorFactory;
    class ISimulatorListener;
    class ISimulator;

    /*!
     * Manages plugins for the simulator context.
     */
    class BLACKCORE_EXPORT CPluginManagerSimulator :
        public BlackCore::IPluginManager,
        public BlackMisc::IPluginStorageProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CPluginManagerSimulator(QObject *parent = nullptr);

        //! \copydoc BlackMisc::IPluginStorageProvider::getPluginData
        virtual BlackMisc::CVariant getPluginData(const QObject *obj, const QString &key) const override;

        //! \copydoc BlackMisc::IPluginStorageProvider::setPluginData
        virtual void setPluginData(const QObject *obj, const QString &key, const BlackMisc::CVariant &value) override;

        //! Get simulator factory from the plugin
        ISimulatorFactory *getFactory(const QString &pluginId);

        //! Get simulator listener from the plugin
        ISimulatorListener *getListener(const QString &pluginId);

        //! Get all simulator driver plugins
        BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const;

        //! \copydoc BlackCore::IPluginManager::collectPlugins()
        virtual void collectPlugins() override;

    protected:
        //! \copydoc BlackCore::IPluginManager::acceptedIids()
        virtual BlackMisc::CSequence<QString> acceptedIids() const override;

        //! \copydoc BlackCore::IPluginManager::pluginDirectory()
        virtual QString pluginDirectory() const override;

    private:
        /*!
         * Extended data for plugin
         */
        struct PluginExtended
        {
            BlackMisc::Simulation::CSimulatorPluginInfo info;
            ISimulatorListener *listener = nullptr;
            QHash<QString, BlackMisc::CVariant> storage; //!< Permanent plugin storage - data stored here will be kept even when plugin is unloaded
        };

        QMap<QString, PluginExtended> m_plugins; //!< Id <-> extended data pairs

    };

} // namespace

#endif // BLACKCORE_PLUGIN_MANAGER_SIMULATOR_H
