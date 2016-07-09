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

#include "blackcore/simulatorcommon.h"
#include "blackmisc/interpolator.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
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

            //! \copydoc BlackCore::ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! FSUIPC connected?
            bool isFsuipcConnected() const;

            //! Experimental model matching
            BlackMisc::Simulation::CAircraftModel getClosestMatch(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! \copydoc BlackCore::ISimulator::isPaused
            virtual bool isPaused() const override { return m_simPaused; }

            //! \copydoc BlackCore::ISimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }

            //! \copydoc BlackCore::ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc BlackCore::ISimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;

            //! \copydoc BlackCore::ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

            //! \copydoc BlackCore::ISimulator::changeRemoteAircraftModel
            virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::ISimulator::changeRemoteAircraftEnabled
            virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::ISimulator::enableDebugMessages
            virtual void enableDebugMessages(bool driver, bool interpolator) override;

        protected:
            //! Constructor
            CSimulatorFsCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                               BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                               BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                               BlackMisc::Weather::IWeatherGridProvider       *weatherGridProvider,
                               QObject *parent = nullptr);

            QString simulatorDetails;                               //!< describes version etc.
            QScopedPointer<FsCommon::CFsuipc> m_fsuipc;             //!< FSUIPC
            bool m_useFsuipc = true;                                //!< use FSUIPC
            bool m_simPaused = false;                               //!< Simulator paused?
            bool m_simTimeSynced = false;                           //!< Time synchronized?
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset;  //!< time offset
            BlackMisc::Aviation::CAirportList    m_airportsInRange; //!< airports in range of own aircraft

            // cockpit as set in SIM
            BlackMisc::Aviation::CComSystem   m_simCom1;            //!< cockpit COM1 state in simulator
            BlackMisc::Aviation::CComSystem   m_simCom2;            //!< cockpit COM2 state in simulator
            BlackMisc::Aviation::CTransponder m_simTransponder;     //!< cockpit xpdr state in simulator

            //! Set own model
            void setOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set own model
            void setOwnAircraftModel(const QString &modelName);
        };

    } // namespace
} // namespace

#endif // guard
