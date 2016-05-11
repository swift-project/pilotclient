/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/ownaircraftprovider.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {

        const CSimulatedAircraft COwnAircraftAware::getOwnAircraft() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->getOwnAircraft();
        }

        CCoordinateGeodetic COwnAircraftAware::getOwnAircraftPosition() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->getOwnAircraftPosition();
        }

        CAircraftParts COwnAircraftAware::getOwnAircraftParts() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->getOwnAircraftParts();
        }

        CAircraftModel COwnAircraftAware::getOwnAircraftModel() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->getOwnAircraftModel();
        }

        CLength COwnAircraftAware::getDistanceToOwnAircraft(const Geo::ICoordinateGeodetic &position) const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->getDistanceToOwnAircraft(position);
        }

        bool COwnAircraftAware::updateCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder, const CIdentifier &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateCockpit(com1, com2, transponder, originator);
        }

        bool COwnAircraftAware::updateActiveComFrequency(const CFrequency &frequency, CComSystem::ComUnit comUnit, const CIdentifier &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateActiveComFrequency(frequency, comUnit, originator);
        }

        bool COwnAircraftAware::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateSelcal(selcal, originator);
        }

        bool COwnAircraftAware::updateOwnModel(const CAircraftModel &model)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnModel(model);
        }

        bool COwnAircraftAware::updateOwnSituation(const CAircraftSituation &situation)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnSituation(situation);
        }

        bool COwnAircraftAware::updateOwnParts(const CAircraftParts &parts)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnParts(parts);
        }

        bool COwnAircraftAware::updateOwnCallsign(const CCallsign &callsign)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnCallsign(callsign);
        }

        bool COwnAircraftAware::updateOwnIcaoCodes(const CAircraftIcaoCode &aircraftIcaoData, const CAirlineIcaoCode &airlineIcaoCode)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnIcaoCodes(aircraftIcaoData, airlineIcaoCode);
        }

    } // namespace
} // namespace
