// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_PLUGIN_MANAGER_SIMULATOR_H
#define SWIFT_CORE_PLUGIN_MANAGER_SIMULATOR_H

#include <QMap>
#include <QObject>
#include <QString>

#include "core/pluginmanager.h"
#include "core/swiftcoreexport.h"
#include "misc/sequence.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/simulation/simulatorplugininfolist.h"

namespace swift::core
{
    class ISimulatorFactory;
    class ISimulatorListener;

    /*!
     * Manages plugins for the simulator context.
     */
    class SWIFT_CORE_EXPORT CPluginManagerSimulator : public swift::core::IPluginManager
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
        swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const;

        //! Check if simulator is connected
        int checkAvailableListeners();

        //! \copydoc swift::core::IPluginManager::collectPlugins()
        virtual void collectPlugins() override;

    protected:
        //! \copydoc swift::core::IPluginManager::acceptedIids()
        virtual swift::misc::CSequence<QString> acceptedIids() const override;

        //! \copydoc swift::core::IPluginManager::pluginDirectory()
        virtual const QString &pluginDirectory() const override;

    private:
        /*!
         * Extended data for plugin
         */
        struct PluginExtended
        {
            swift::misc::simulation::CSimulatorPluginInfo info;
            ISimulatorListener *listener = nullptr;
        };

        QMap<QString, PluginExtended> m_plugins; //!< Id <-> extended data pairs
    };
} // namespace swift::core

#endif // guard
