// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_H
#define BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_H

#include "plugins/simulator/plugincommon/simulatorplugincommon.h"
#include "blackmisc/simulation/interpolation/interpolator.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "plugins/simulator/fscommon/fscommonexport.h"

#include <QObject>
#include <memory>

namespace BlackSimPlugin::FsCommon
{
    //! Common base class for MS flight simulators
    class FSCOMMON_EXPORT CSimulatorFsCommon : public Common::CSimulatorPluginCommon
    {
        Q_OBJECT
        Q_INTERFACES(BlackCore::ISimulator)
        Q_INTERFACES(BlackMisc::Simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(BlackMisc::Simulation::IInterpolationSetupProvider)

    public:
        //! Destructor
        virtual ~CSimulatorFsCommon() override;

        // ---------------------- ISimulator ------------------
        virtual bool disconnectFrom() override;
        virtual bool isPaused() const override { return m_simPaused; }
        virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;
        virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;
        // ---------------------- ISimulator ------------------

    protected:
        //! Constructor
        CSimulatorFsCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                           BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                           BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                           BlackMisc::Network::IClientProvider *clientProvider,
                           QObject *parent = nullptr);

        //! Init the internal objects
        virtual void initSimulatorInternals() override;

        //! When swift DB data are read
        virtual void onSwiftDbAirportsRead() override;

        //! \copydoc BlackCore::ISimulator::reset
        virtual void reset() override;

        //! Register help
        static void registerHelp();

        int m_ownAircraftUpdateCycles = 0; //!< own aircraft updates, even with 50 updates/sec long enough even for 32bit
        int m_skipCockpitUpdateCycles = 0; //!< skip some update cycles to allow changes in simulator cockpit to be set
        bool m_simPaused = false; //!< simulator paused?
        bool m_simTimeSynced = false; //!< time synchronized?
        BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset; //!< time offset
        BlackMisc::Aviation::CAirportList m_airportsInRangeFromSimulator; //!< airports in range of own aircraft

        // cockpit as set in SIM
        BlackMisc::Aviation::CComSystem m_simCom1; //!< cockpit COM1 state in simulator
        BlackMisc::Aviation::CComSystem m_simCom2; //!< cockpit COM2 state in simulator
        BlackMisc::Aviation::CTransponder m_simTransponder; //!< cockpit xpdr state in simulator
        BlackMisc::Aviation::CSelcal m_selcal; //!< SELCAL as in cockpit
    };
} // namespace

#endif // guard
