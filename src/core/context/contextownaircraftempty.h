// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_EMPTY_H
#define SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_EMPTY_H

#include "core/context/contextownaircraft.h"
#include "core/swiftcoreexport.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::core::context
{
    //! Empty context, used during shutdown/initialization
    class SWIFT_CORE_EXPORT CContextOwnAircraftEmpty : public IContextOwnAircraft
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextOwnAircraftEmpty(CCoreFacade *runtime) : IContextOwnAircraft(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatedAircraft();
        }

        //! \copydoc IContextOwnAircraft::getOwnComSystem
        swift::misc::aviation::CComSystem
        getOwnComSystem(swift::misc::aviation::CComSystem::ComUnit unit) const override
        {
            Q_UNUSED((unit);)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CComSystem();
        }

        //! \copydoc IContextOwnAircraft::getOwnTransponder()
        swift::misc::aviation::CTransponder getOwnTransponder() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CTransponder();
        }

        //! \copydoc IContextOwnAircraft::getOwnAircraftSituation()
        swift::misc::aviation::CAircraftSituation getOwnAircraftSituation() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAircraftSituation();
        }

        //! \copydoc IContextOwnAircraft::updateOwnPosition
        bool updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position,
                               const swift::misc::aviation::CAltitude &altitude,
                               const swift::misc::aviation::CAltitude &pressureAltitude) override
        {
            Q_UNUSED(position);
            Q_UNUSED(altitude);
            Q_UNUSED(pressureAltitude)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateCockpit
        bool updateCockpit(const swift::misc::aviation::CComSystem &com1, const swift::misc::aviation::CComSystem &com2,
                           const swift::misc::aviation::CTransponder &transponder,
                           const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(com1);
            Q_UNUSED(com2);
            Q_UNUSED(transponder);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateTransponderMode
        bool updateTransponderMode(const swift::misc::aviation::CTransponder::TransponderMode &transponderMode,
                                   const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(transponderMode);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateActiveComFrequency
        bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                      swift::misc::aviation::CComSystem::ComUnit comUnit,
                                      const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(frequency);
            Q_UNUSED(comUnit);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnAircraftPilot
        bool updateOwnAircraftPilot(const swift::misc::network::CUser &pilot) override
        {
            Q_UNUSED(pilot);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateSelcal
        bool updateSelcal(const swift::misc::aviation::CSelcal &selcal,
                          const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(selcal);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnCallsign
        bool updateOwnCallsign(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
        bool updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode) override
        {
            Q_UNUSED(aircraftIcaoCode);
            Q_UNUSED(airlineIcaoCode);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::toggleTransponderMode
        void toggleTransponderMode() override { logEmptyContextWarning(Q_FUNC_INFO); }

        //! \copydoc IContextOwnAircraft::setTransponderMode
        bool setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode) override
        {
            Q_UNUSED(mode);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::parseCommandLine
        bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }
    };
} // namespace swift::core::context
#endif // SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_EMPTY_H
