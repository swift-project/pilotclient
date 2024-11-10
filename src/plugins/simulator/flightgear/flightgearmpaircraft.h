// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FLIGHTGEAR_FGPLANEMPAIRCRAFT_H
#define BLACKSIMPLUGIN_FLIGHTGEAR_FGPLANEMPAIRCRAFT_H

#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/interpolation/interpolatormulti.h"
#include <QSharedPointer>
#include <QStringList>

namespace swift::core
{
    class ISimulator;
}
namespace BlackSimPlugin::Flightgear
{
    //! Class representing a Flightgear multiplayer aircraft
    class CFlightgearMPAircraft
    {
    public:
        //! Constructor
        CFlightgearMPAircraft();

        //! Constructor providing initial situation/parts
        CFlightgearMPAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                              swift::core::ISimulator *simulator,
                              swift::misc::simulation::CInterpolationLogger *logger);

        //! Destructor
        ~CFlightgearMPAircraft() {}

        //! Set simulated aircraft
        void setSimulatedAircraft(const swift::misc::simulation::CSimulatedAircraft &simulatedAircraft);

        //! Get callsign
        const swift::misc::aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

        //! Has callsign
        bool hasCallsign() const { return m_aircraft.hasCallsign(); }

        //! Simulated aircraft (as added)
        const swift::misc::simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

        //! Simulated aircraft model string
        const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

        //! Simulated aircraft model
        const swift::misc::simulation::CAircraftModel &getAircraftModel() const { return m_aircraft.getModel(); }

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolatorInfo
        QString getInterpolatorInfo(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc swift::misc::simulation::CInterpolator::attachLogger
        void attachInterpolatorLogger(swift::misc::simulation::CInterpolationLogger *logger) const;

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolation
        swift::misc::simulation::CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoch, const swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign &setup, uint32_t aircraftNumber) const;

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolationMessages
        swift::misc::CStatusMessageList getInterpolationMessages(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! Interpolator
        swift::misc::simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

    private:
        swift::misc::simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
        QSharedPointer<swift::misc::simulation::CInterpolatorMulti> m_interpolator; //!< shared pointer because CSimConnectObject can be copied
    };

    //! Simulator objects (aka AI aircraft)
    class CFlightgearMPAircraftObjects : public QHash<swift::misc::aviation::CCallsign, CFlightgearMPAircraft>
    {
    public:
        //! Get all callsigns
        swift::misc::aviation::CCallsignSet getAllCallsigns() const;

        //! Get all callsign strings
        QStringList getAllCallsignStrings(bool sorted = false) const;

        //! Get all callsign strings as string
        QString getAllCallsignStringsAsString(bool sorted = false, const QString &separator = ", ") const;

        //! Toggle interpolator modes
        void toggleInterpolatorModes();

        //! Toggle interpolator modes
        void toggleInterpolatorMode(const swift::misc::aviation::CCallsign &callsign);
    };
} // namespace

#endif // guard
