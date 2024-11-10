// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_H
#define SWIFT_SIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_H

#include "plugins/simulator/plugincommon/simulatorplugincommon.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/simulation/fscommon/aircraftcfgparser.h"
#include "plugins/simulator/fscommon/fscommonexport.h"

#include <QObject>
#include <memory>

namespace swift::simplugin::fscommon
{
    //! Common base class for MS flight simulators
    class FSCOMMON_EXPORT CSimulatorFsCommon : public common::CSimulatorPluginCommon
    {
        Q_OBJECT
        Q_INTERFACES(swift::core::ISimulator)
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

    public:
        //! Destructor
        virtual ~CSimulatorFsCommon() override;

        // ---------------------- ISimulator ------------------
        virtual bool disconnectFrom() override;
        virtual bool isPaused() const override { return m_simPaused; }
        virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }
        virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override;
        virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) override;
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;
        // ---------------------- ISimulator ------------------

    protected:
        //! Constructor
        CSimulatorFsCommon(const swift::misc::simulation::CSimulatorPluginInfo &info,
                           swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                           swift::misc::simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                           swift::misc::network::IClientProvider *clientProvider,
                           QObject *parent = nullptr);

        //! Init the internal objects
        virtual void initSimulatorInternals() override;

        //! When swift DB data are read
        virtual void onSwiftDbAirportsRead() override;

        //! \copydoc swift::core::ISimulator::reset
        virtual void reset() override;

        //! Register help
        static void registerHelp();

        int m_ownAircraftUpdateCycles = 0; //!< own aircraft updates, even with 50 updates/sec long enough even for 32bit
        int m_skipCockpitUpdateCycles = 0; //!< skip some update cycles to allow changes in simulator cockpit to be set
        bool m_simPaused = false; //!< simulator paused?
        bool m_simTimeSynced = false; //!< time synchronized?
        swift::misc::physical_quantities::CTime m_syncTimeOffset; //!< time offset
        swift::misc::aviation::CAirportList m_airportsInRangeFromSimulator; //!< airports in range of own aircraft

        // cockpit as set in SIM
        swift::misc::aviation::CComSystem m_simCom1; //!< cockpit COM1 state in simulator
        swift::misc::aviation::CComSystem m_simCom2; //!< cockpit COM2 state in simulator
        swift::misc::aviation::CTransponder m_simTransponder; //!< cockpit xpdr state in simulator
        swift::misc::aviation::CSelcal m_selcal; //!< SELCAL as in cockpit
    };
} // namespace

#endif // guard
