/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ownaircraftprovider.h"
#include "blackmisc/aviation/aircraft.h"

using namespace BlackMisc::Aviation;
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

        Geo::CCoordinateGeodetic COwnAircraftAware::getOwnAircraftPosition() const
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

        bool COwnAircraftAware::updateCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder, const QString &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateCockpit(com1, com2, transponder, originator);
        }

        bool COwnAircraftAware::updateActiveComFrequency(const CFrequency &frequency, int comUnit, const QString &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateActiveComFrequency(frequency, comUnit, originator);
        }

        bool COwnAircraftAware::updateSelcal(const CSelcal &selcal, const QString &originator)
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

        bool COwnAircraftAware::updateOwnIcaoData(const CAircraftIcao &icaoData)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_ownAircraftProvider->updateOwnIcaoData(icaoData);
        }

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

        bool COwnAircraftProviderDummy::updateCockpit(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder, const QString &originator)
        {
            m_ownAircraft.setCom1System(com1);
            m_ownAircraft.setCom2System(com2);
            m_ownAircraft.setTransponder(transponder);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateActiveComFrequency(const PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            CComSystem::ComUnit comUnitEnum = static_cast<CComSystem::ComUnit>(comUnit);
            CComSystem com = m_ownAircraft.getComSystem(comUnitEnum);
            com.setFrequencyActive(frequency);
            m_ownAircraft.setComSystem(com, comUnitEnum);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateSelcal(const CSelcal &selcal, const QString &originator)
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

        bool COwnAircraftProviderDummy::updateOwnIcaoData(const CAircraftIcao &icaoData)
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
