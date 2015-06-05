/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ownaircraftproviderdummy.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {

        Geo::CCoordinateGeodetic COwnAircraftProviderDummy::getOwnAircraftPosition() const
        {
            return m_ownAircraft.getPosition();
        }

        CAircraftParts COwnAircraftProviderDummy::getOwnAircraftParts() const
        {
            return m_ownAircraft.getParts();
        }

        CAircraftModel COwnAircraftProviderDummy::getOwnAircraftModel() const
        {
            return m_ownAircraft.getModel();
        }

        CLength COwnAircraftProviderDummy::getDistanceToOwnAircraft(const Geo::ICoordinateGeodetic &position) const
        {
            return m_ownAircraft.calculateGreatCircleDistance(position);
        }

        bool COwnAircraftProviderDummy::updateCockpit(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder, const CIdentifier &originator)
        {
            m_ownAircraft.setCom1System(com1);
            m_ownAircraft.setCom2System(com2);
            m_ownAircraft.setTransponder(transponder);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateActiveComFrequency(const PhysicalQuantities::CFrequency &frequency, int comUnit, const BlackMisc::CIdentifier &originator)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            CComSystem::ComUnit comUnitEnum = static_cast<CComSystem::ComUnit>(comUnit);
            CComSystem com = m_ownAircraft.getComSystem(comUnitEnum);
            com.setFrequencyActive(frequency);
            m_ownAircraft.setComSystem(com, comUnitEnum);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateSelcal(const CSelcal &selcal, const BlackMisc::CIdentifier &originator)
        {
            m_ownAircraft.setSelcal(selcal);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateOwnModel(const CAircraftModel &model)
        {
            m_ownAircraft.setModel(model);
            return true;
        }

        bool COwnAircraftProviderDummy::updateOwnSituation(const CAircraftSituation &situation)
        {
            m_ownAircraft.setSituation(situation);
            return true;
        }

        bool COwnAircraftProviderDummy::updateOwnParts(const CAircraftParts &parts)
        {
            m_ownAircraft.setParts(parts);
            return true;
        }

        bool COwnAircraftProviderDummy::updateOwnCallsign(const CCallsign &callsign)
        {
            m_ownAircraft.setCallsign(callsign);
            return true;
        }

        bool COwnAircraftProviderDummy::updateOwnIcaoData(const CAircraftIcaoData &icaoData)
        {
            m_ownAircraft.setIcaoInfo(icaoData);
            return true;
        }

        COwnAircraftProviderDummy *COwnAircraftProviderDummy::instance()
        {
            static COwnAircraftProviderDummy *dummy = new COwnAircraftProviderDummy();
            return dummy;
        }

    } // namespace
} // namespace
