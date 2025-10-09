// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/ownaircraftproviderdummy.h"

#include <QtGlobal>

using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace swift::misc::simulation
{
    CSimulatedAircraft COwnAircraftProviderDummy::getOwnAircraft() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft;
    }

    CCallsign COwnAircraftProviderDummy::getOwnCallsign() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getCallsign();
    }

    CComSystem COwnAircraftProviderDummy::getOwnComSystem(CComSystem::ComUnit unit) const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getComSystem(unit);
    }

    CTransponder COwnAircraftProviderDummy::getOwnTransponder() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getTransponder();
    }

    geo::CCoordinateGeodetic COwnAircraftProviderDummy::getOwnAircraftPosition() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getPosition();
    }

    CAircraftSituation COwnAircraftProviderDummy::getOwnAircraftSituation() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getSituation();
    }

    CAircraftParts COwnAircraftProviderDummy::getOwnAircraftParts() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getParts();
    }

    CAircraftModel COwnAircraftProviderDummy::getOwnAircraftModel() const
    {
        QReadLocker l(&m_lock);
        return m_ownAircraft.getModel();
    }

    CLength COwnAircraftProviderDummy::getDistanceToOwnAircraft(const geo::ICoordinateGeodetic &position) const
    {
        return getOwnAircraft().calculateGreatCircleDistance(position);
    }

    bool COwnAircraftProviderDummy::updateCockpit(const aviation::CComSystem &com1, const aviation::CComSystem &com2,
                                                  const aviation::CTransponder &transponder,
                                                  const CIdentifier &originator)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setCom1System(com1);
        m_ownAircraft.setCom2System(com2);
        m_ownAircraft.setTransponder(transponder);
        Q_UNUSED(originator);
        return true;
    }

    bool COwnAircraftProviderDummy::updateActiveComFrequency(const physical_quantities::CFrequency &frequency,
                                                             CComSystem::ComUnit comUnit,
                                                             const swift::misc::CIdentifier &originator)
    {
        if (!CComSystem::isValidComFrequency(frequency)) { return false; }
        QWriteLocker l(&m_lock);
        CComSystem com = m_ownAircraft.getComSystem(comUnit);
        com.setFrequencyActive(frequency);
        m_ownAircraft.setComSystem(com, comUnit);
        Q_UNUSED(originator);
        return true;
    }

    bool COwnAircraftProviderDummy::updateSelcal(const CSelcal &selcal, const swift::misc::CIdentifier &originator)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setSelcal(selcal);
        Q_UNUSED(originator);
        return true;
    }

    bool COwnAircraftProviderDummy::updateOwnModel(const CAircraftModel &model)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setModel(model);
        return true;
    }

    bool COwnAircraftProviderDummy::updateOwnSituation(const CAircraftSituation &situation)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setSituation(situation);
        return true;
    }

    bool COwnAircraftProviderDummy::updateOwnParts(const CAircraftParts &parts)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setParts(parts);
        return true;
    }

    bool COwnAircraftProviderDummy::updateOwnCG(const CLength &cg)
    {
        QWriteLocker l(&m_lock);
        return m_ownAircraft.setCG(cg);
    }

    bool COwnAircraftProviderDummy::updateOwnCallsign(const CCallsign &callsign)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setCallsign(callsign);
        return true;
    }

    bool COwnAircraftProviderDummy::updateOwnIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode,
                                                       const CAirlineIcaoCode &airlineIcaoCode)
    {
        QWriteLocker l(&m_lock);
        m_ownAircraft.setIcaoCodes(aircraftIcaoCode, airlineIcaoCode);
        return true;
    }

    COwnAircraftProviderDummy *COwnAircraftProviderDummy::instance()
    {
        static auto dummy = new COwnAircraftProviderDummy();
        return dummy;
    }
} // namespace swift::misc::simulation
