/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/ownaircraftprovider.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Simulation
{
    bool IOwnAircraftProvider::updateCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
    {
        const bool changed1 = this->updateCockpit(aircraft.getCom1System(), aircraft.getCom2System(), aircraft.getTransponder(), originator);
        const bool changed2 = this->updateSelcal(aircraft.getSelcal(), originator);
        return changed1 || changed2;
    }

    // pin vtables to this file
    void COwnAircraftAware::anchor()
    {}

    CSimulatedAircraft COwnAircraftAware::getOwnAircraft() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnAircraft();
    }

    CCallsign COwnAircraftAware::getOwnCallsign() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnCallsign();
    }

    CCoordinateGeodetic COwnAircraftAware::getOwnAircraftPosition() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnAircraftPosition();
    }

    CAircraftSituation COwnAircraftAware::getOwnAircraftSituation() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnAircraftSituation();
    }

    CAircraftParts COwnAircraftAware::getOwnAircraftParts() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnAircraftParts();
    }

    CAircraftModel COwnAircraftAware::getOwnAircraftModel() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getOwnAircraftModel();
    }

    CLength COwnAircraftAware::getDistanceToOwnAircraft(const Geo::ICoordinateGeodetic &position) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getDistanceToOwnAircraft(position);
    }

    bool COwnAircraftAware::updateCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateCockpit(aircraft, originator);
    }

    bool COwnAircraftAware::updateCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateCockpit(com1, com2, transponder, originator);
    }

    bool COwnAircraftAware::updateActiveComFrequency(const CFrequency &frequency, CComSystem::ComUnit comUnit, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateActiveComFrequency(frequency, comUnit, originator);
    }

    bool COwnAircraftAware::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateSelcal(selcal, originator);
    }

    bool COwnAircraftAware::updateOwnModel(const CAircraftModel &model)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnModel(model);
    }

    bool COwnAircraftAware::updateOwnSituation(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnSituation(situation);
    }

    bool COwnAircraftAware::updateOwnParts(const CAircraftParts &parts)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnParts(parts);
    }

    bool COwnAircraftAware::updateOwnCG(const CLength &cg)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnCG(cg);
    }

    bool COwnAircraftAware::updateOwnCallsign(const CCallsign &callsign)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnCallsign(callsign);
    }

    bool COwnAircraftAware::updateOwnIcaoCodes(const CAircraftIcaoCode &aircraftIcaoData, const CAirlineIcaoCode &airlineIcaoCode)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateOwnIcaoCodes(aircraftIcaoData, airlineIcaoCode);
    }
} // namespace
