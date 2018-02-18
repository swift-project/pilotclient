/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_XPLANE_XPLANEMPAIRCRAFT_H
#define BLACKSIMPLUGIN_XPLANE_XPLANEMPAIRCRAFT_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include <QSharedPointer>
#include <QStringList>

namespace BlackSimPlugin
{
    namespace XPlane
    {
        //! Class representing a X-Plane multiplayer aircraft
        class CXPlaneMPAircraft
        {
        public:
            //! Constructor
            CXPlaneMPAircraft();

            //! Constructor providing initial situation/parts
            CXPlaneMPAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft,
                              BlackMisc::Simulation::CInterpolationLogger *logger);

            //! Constructor providing initial situation
            CXPlaneMPAircraft(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Constructor providing initial parts
            CXPlaneMPAircraft(const BlackMisc::Aviation::CAircraftParts &parts, const BlackMisc::Aviation::CCallsign &callsign);

            //! Destructor
            ~CXPlaneMPAircraft() {}

            //! Get callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

            //! Simulated aircraft (as added)
            const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

            //! Simulated aircraft model string
            const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

            //! Set the aircraft
            void setAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) { m_aircraft = aircraft; }

            //! Add parts for interpolator
            void addAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Add situation for interpolator
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Parts as sent to simulator
            const BlackMisc::Aviation::CAircraftParts &getPartsAsSent() const { return m_partsAsSent; }

            //! Parts as sent to simulator
            void setPartsAsSent(const BlackMisc::Aviation::CAircraftParts &parts) { m_partsAsSent = parts; }

            //! Position as sent
            void setPositionAsSent(const BlackMisc::Aviation::CAircraftSituation &position) { m_positionAsSent = position; }

            //! Same as sent
            bool isSameAsSent(const BlackMisc::Aviation::CAircraftSituation &position) const;

            //! VTOL?
            bool isVtol() const { return m_aircraft.isVtol(); }

            //! \copydoc BlackMisc::Simulation::CInterpolatorMulti::toggleMode
            void toggleInterpolatorMode();

            //! \copydoc BlackMisc::Simulation::CInterpolatorMulti::setMode
            bool setInterpolatorMode(BlackMisc::Simulation::CInterpolatorMulti::Mode mode);

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolatorInfo
            QString getInterpolatorInfo() const;

            //! \copydoc BlackMisc::Simulation::CInterpolator::attachLogger
            void attachInterpolatorLogger(BlackMisc::Simulation::CInterpolationLogger *logger);

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolatedSituation
            BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                qint64 currentTimeSinceEpoc,
                const BlackMisc::Simulation::CInterpolationAndRenderingSetup &setup,
                const BlackMisc::Simulation::CInterpolationHints &hints, BlackMisc::Simulation::CInterpolationStatus &status) const;

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolatedParts
            BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                qint64 currentTimeSinceEpoc,
                const BlackMisc::Simulation::CInterpolationAndRenderingSetup &setup,
                BlackMisc::Simulation::CPartsStatus &partsStatus, bool log) const;

            //! Interpolator
            BlackMisc::Simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
            QSharedPointer<BlackMisc::Simulation::CInterpolatorMulti> m_interpolator; //!< shared pointer because CSimConnectObject can be copied
            BlackMisc::Aviation::CAircraftSituation m_positionAsSent;
            BlackMisc::Aviation::CAircraftParts m_partsAsSent;
        };

        //! Simulator objects (aka AI aircraft)
        class CXPlaneMPAircrafts : public QHash<BlackMisc::Aviation::CCallsign, CXPlaneMPAircraft>
        {
        public:
            //! Get all callsigns
            BlackMisc::Aviation::CCallsignSet getAllCallsigns() const;

            //! Get all callsign strings
            QStringList getAllCallsignStrings(bool sorted = false) const;

            //! Get all callsign strings as string
            QString getAllCallsignStringsAsString(bool sorted = false, const QString &separator = ", ") const;

            //! Toggle interpolator modes
            void toggleInterpolatorModes();

            //! Toggle interpolator modes
            void toggleInterpolatorMode(const BlackMisc::Aviation::CCallsign &callsign);

            //! Set interpolator modes
            int setInterpolatorModes(BlackMisc::Simulation::CInterpolatorMulti::Mode mode);
        };
    } // namespace
} // namespace

#endif // guard
