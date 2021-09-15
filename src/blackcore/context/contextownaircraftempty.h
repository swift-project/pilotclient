/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_EMPTY_H
#define BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_EMPTY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackmisc/logmessage.h"

// clazy:excludeall=const-signal-or-slot

namespace BlackCore::Context
{
    //! Empty context, used during shutdown/initialization
    class BLACKCORE_EXPORT CContextOwnAircraftEmpty : public IContextOwnAircraft
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextOwnAircraftEmpty(CCoreFacade *runtime) : IContextOwnAircraft(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatedAircraft();
        }

        //! \copydoc IContextOwnAircraft::getOwnComSystem
        virtual BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const override
        {
            Q_UNUSED((unit);)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CComSystem();
        }

        //! \copydoc IContextOwnAircraft::getOwnTransponder()
        virtual BlackMisc::Aviation::CTransponder getOwnTransponder() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CTransponder();
        }

        //! \copydoc IContextOwnAircraft::getOwnAircraftSituation()
        virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAircraftSituation();
        }

        //! \copydoc IContextOwnAircraft::updateOwnPosition
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude) override
        {
            Q_UNUSED(position);
            Q_UNUSED(altitude);
            Q_UNUSED(pressureAltitude)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateCockpit
        virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(com1);
            Q_UNUSED(com2);
            Q_UNUSED(transponder);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateTransponderMode
        virtual bool updateTransponderMode(const BlackMisc::Aviation::CTransponder::TransponderMode &transponderMode, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(transponderMode);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateActiveComFrequency
        virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(frequency);
            Q_UNUSED(comUnit);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnAircraftPilot
        virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override
        {
            Q_UNUSED(pilot);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateSelcal
        virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(selcal);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnCallsign
        virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
        virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override
        {
            Q_UNUSED(aircraftIcaoCode);
            Q_UNUSED(airlineIcaoCode);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::toggleTransponderMode
        virtual void toggleTransponderMode() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextOwnAircraft::setTransponderMode
        virtual bool setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) override
        {
            Q_UNUSED(mode);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }
    };
} // namespace
#endif // guard
