/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_XPLANE_XPLANEMPAIRCRAFT_H
#define BLACKSIMPLUGIN_XPLANE_XPLANEMPAIRCRAFT_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include <QSharedPointer>
#include <QStringList>

namespace BlackCore
{
    class ISimulator;
}
namespace BlackSimPlugin::XPlane
{
    //! Class representing a X-Plane multiplayer aircraft
    class CXPlaneMPAircraft
    {
    public:
        //! Constructor
        CXPlaneMPAircraft();

        //! Constructor providing initial situation/parts
        CXPlaneMPAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft,
                          BlackCore::ISimulator *simulator,
                          BlackMisc::Simulation::CInterpolationLogger *logger);

        //! Destructor
        ~CXPlaneMPAircraft() {}

        //! Set simulated aircraft
        void setSimulatedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &simulatedAircraft);

        //! Get callsign
        const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

        //! Has callsign
        bool hasCallsign() const { return m_aircraft.hasCallsign(); }

        //! Simulated aircraft (as added)
        const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

        //! Simulated aircraft model
        const BlackMisc::Simulation::CAircraftModel &getAircraftModel() const { return m_aircraft.getModel(); }

        //! Simulated aircraft model string
        const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

        //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolatorInfo
        QString getInterpolatorInfo(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc BlackMisc::Simulation::CInterpolator::attachLogger
        void attachInterpolatorLogger(BlackMisc::Simulation::CInterpolationLogger *logger) const;

        //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolation
        BlackMisc::Simulation::CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoc, const BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber) const;

        //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolationMessages
        BlackMisc::CStatusMessageList getInterpolationMessages(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! Interpolator
        BlackMisc::Simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

    private:
        BlackMisc::Simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
        QSharedPointer<BlackMisc::Simulation::CInterpolatorMulti> m_interpolator; //!< shared pointer because CSimConnectObject can be copied
    };

    //! Simulator objects (aka AI aircraft)
    class CXPlaneMPAircraftObjects : public QHash<BlackMisc::Aviation::CCallsign, CXPlaneMPAircraft>
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
    };
} // namespace

#endif // guard
