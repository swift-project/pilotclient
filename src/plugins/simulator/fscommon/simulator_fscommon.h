/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_COMMON_H
#define BLACKSIMPLUGIN_SIMULATOR_COMMON_H

#include "blackcore/simulator.h"
#include "blacksim/fscommon/aircraftmapper.h"
#include "fsuipc.h"

#include <QObject>

namespace BlackSimPlugin
{
    namespace FsCommon
    {
        //! Common base class for MS flight simulators
        class CSimulatorFsCommon : public BlackCore::ISimulator, public BlackMisc::Simulation::COwnAircraftProviderSupport
        {
        public:
            //! Destructor
            virtual ~CSimulatorFsCommon();

            //! \copydoc ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! FSUIPC connected?
            bool isFsuipcConnected() const;

            //! Experimental model matching
            static BlackMisc::Simulation::CAircraftModel modelMatching(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! SimObjects directory
            static QString simObjectsDir();

        public slots:

            //! \copydoc ISimulator::isPaused
            virtual bool isPaused() const override { return m_simPaused; }

            //! \copydoc ISimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }

            //! \copydoc ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc ISimulator::setTimeSynchronization
            virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! \copydoc ISimulator::getSimulatorInfo
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

            //! \copydoc BlackCore::ISimulator::getRemoteAircraft
            virtual BlackMisc::Simulation::CSimulatedAircraftList getRemoteAircraft() const override;

            //! \copydoc ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! \copydoc IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        protected:
            //! Constructor
            CSimulatorFsCommon(const BlackSim::CSimulatorInfo &simInfo, BlackMisc::Simulation::IOwnAircraftProvider *ownAircraft, QObject *parent = nullptr);

            QString simulatorDetails;                       //!< describes version etc.
            BlackSim::CSimulatorInfo m_simulatorInfo;       //!< about the simulator
            QScopedPointer<FsCommon::CFsuipc> m_fsuipc;     //!< FSUIPC
            bool m_simPaused = false;                       //!< Simulator paused?
            bool m_simTimeSynced = false;                   //!< Time synchronized?
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset; //!< time offset
            BlackMisc::Simulation::CSimulatedAircraftList m_remoteAircraft; //!< mapped models
            BlackMisc::Aviation::CAirportList m_airportsInRange;            //!< airports in range

            // cockpit as set in SIM
            BlackMisc::Aviation::CComSystem  m_simCom1;  //!< cockpit COM1 state in simulator
            BlackMisc::Aviation::CComSystem  m_simCom2;  //!< cockpit COM2 state in simulator
            BlackMisc::Aviation::CTransponder m_simTransponder; //!< cockpit xpdr state in simulator

            //! Set own model
            void setOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Own aircraft has to be be changed
            void setOwnAircraftModel(const QString &modelName);

            //! Get the mapper singleton
            static BlackSim::FsCommon::CAircraftMapper *mapperInstance();

        protected slots:

            //! Mapper has been initialized
            void ps_mapperInitialized(bool success);
        };

    } // namespace
} // namespace

#endif // guard
