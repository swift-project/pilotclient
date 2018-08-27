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

#include "blackcore/blackcoreexport.h"
#include "blackcore/pluginmanager.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/variant.h"

#include <QHash>
#include <QMap>
#include <QObject>
#include <QString>

namespace BlackCore
{
    class ISimulatorFactory;
    class ISimulatorListener;

    /*!
     * Manages plugins for the simulator context.
     */
    class BLACKCORE_EXPORT CPluginManagerSimulator : public BlackCore::IPluginManager
    {
        Q_OBJECT

    public:
        //! Ctor
        CPluginManagerSimulator(QObject *parent = nullptr);

        //! Get simulator factory from the plugin
        ISimulatorFactory *getFactory(const QString &pluginId);

        //! Create simulator listener from the plugin
        //! In case one is existing already, it is returned instead.
        //! Returns nullptr if no listener could be created.
        ISimulatorListener *createListener(const QString &pluginId);

        //! Get previously created simulator listener from the plugin
        //! Returns nullptr if listener is not yet created
        ISimulatorListener *getListener(const QString &pluginId);

        //! Get all simulator driver plugins
        BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const;

        //! Check if simulator is connected
        int checkAvailableListeners();

        //! \copydoc BlackCore::IPluginManager::collectPlugins()
        virtual void collectPlugins() override;

    protected:
        //! \copydoc BlackCore::IPluginManager::acceptedIids()
        virtual BlackMisc::CSequence<QString> acceptedIids() const override;

        //! \copydoc BlackCore::IPluginManager::pluginDirectory()
        virtual const QString &pluginDirectory() const override;

    private:
        /*!
         * Extended data for plugin
         */
        struct PluginExtended
        {
            BlackMisc::Simulation::CSimulatorPluginInfo info;
            ISimulatorListener *listener = nullptr;
        };

        QMap<QString, PluginExtended> m_plugins; //!< Id <-> extended data pairs
    };
} // namespace

#endif // guard
