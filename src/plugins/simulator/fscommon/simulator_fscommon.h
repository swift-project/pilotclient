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

#include "blackcore/simulator_common.h"
#include "blackcore/interpolator.h"
#include "blackmisc/simulation/fscommon/aircraftmapper.h"
#include "fsuipc.h"

#include <QObject>

namespace BlackSimPlugin
{
    namespace FsCommon
    {
        //! Common base class for MS flight simulators
        class CSimulatorFsCommon : public BlackCore::CSimulatorCommon
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

            //! \copydoc ISimulator::isPaused
            virtual bool isPaused() const override { return m_simPaused; }

            //! \copydoc ISimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }

            //! \copydoc ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc ISimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! \copydoc ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc ISimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! \copydoc ISimulator::getIcaoForModelString
            virtual BlackMisc::Aviation::CAircraftIcao getIcaoForModelString(const QString &modelString) const override;

            //! \copydoc ISimulator::reloadInstalledModels
            virtual void reloadInstalledModels() override;

            //! \copydoc IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

            //! \copydoc ISimulator::changeRenderedAircraftModel
            virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) override;

            //! \copydoc ISimulator::changeAircraftEnabled
            virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) override;

            //! \copydoc ISimulator::enableDebuggingMessages
            virtual void enableDebugMessages(bool driver, bool interpolator) override;

        protected:
            //! Constructor
            CSimulatorFsCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                QObject *parent = nullptr);

            QString simulatorDetails;                       //!< describes version etc.
            QScopedPointer<FsCommon::CFsuipc> m_fsuipc;     //!< FSUIPC
            bool m_useFsuipc = true;                        //!< use FSUIPC
            bool m_simPaused = false;                       //!< Simulator paused?
            bool m_simTimeSynced = false;                   //!< Time synchronized?
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset; //!< time offset
            BlackMisc::Aviation::CAirportList m_airportsInRange;   //!< aiports in range of own aircraft

            // cockpit as set in SIM
            BlackMisc::Aviation::CComSystem  m_simCom1;  //!< cockpit COM1 state in simulator
            BlackMisc::Aviation::CComSystem  m_simCom2;  //!< cockpit COM2 state in simulator
            BlackMisc::Aviation::CTransponder m_simTransponder; //!< cockpit xpdr state in simulator

            //! Set own model
            void setOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set own model
            void setOwnAircraftModel(const QString &modelName);

            //! Reverse lookup
            static void reverseLookupIcaoData(BlackMisc::Simulation::CAircraftModel &model);

            //! Get the mapper singleton
            static BlackMisc::Simulation::FsCommon::CAircraftMapper *mapperInstance();

        protected slots:

            //! Mapper has been initialized
            void ps_mapperInitialized(bool success);
        };

    } // namespace
} // namespace

#endif // guard
