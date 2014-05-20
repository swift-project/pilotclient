/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_ownaircraft_impl.h"
#include "context_runtime.h"
#include "context_settings.h"


using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextOwnAircraft::CContextOwnAircraft(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextOwnAircraft(mode, runtime)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init own aircraft
        this->initOwnAircraft();

        // 2. connect signals and slots
    }

    /*
     * Cleanup
     */
    CContextOwnAircraft::~CContextOwnAircraft()
    {
    }

    /*
     * Init own aircraft
     */
    void CContextOwnAircraft::initOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
        this->m_ownAircraft.initComSystems();
        this->m_ownAircraft.initTransponder();
        CAircraftSituation situation(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CLength(0, CLengthUnit::m())),
            CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
        );
        this->m_ownAircraft.setSituation(situation);
        this->m_ownAircraft.setPilot(this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer().getUser());

        // TODO: This would need to come from somewhere (mappings)
        // Own callsign, plane ICAO status, model used
        this->m_ownAircraft.setCallsign(CCallsign("BLACK"));
        this->m_ownAircraft.setIcaoInfo(CAircraftIcao("C172", "L1P", "GA", "GA", "0000ff"));
    }

    /*
     * Own Aircraft
     */
    CStatusMessageList CContextOwnAircraft::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        this->m_ownAircraft = aircraft;
        return CStatusMessageList();
    }

    /*
     * Own position
     */
    void CContextOwnAircraft::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        if (this->getRuntime()->isSlotLogForOwnAircraftEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, position.toQString(), altitude.toQString());
        this->m_ownAircraft.setPosition(position);
        this->m_ownAircraft.setAltitude(altitude);
    }

    /*
     * Update own situation
     */
    void CContextOwnAircraft::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        if (this->getRuntime()->isSlotLogForOwnAircraftEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, situation.toQString());
        this->m_ownAircraft.setSituation(situation);
    }

    /*
     * Own cockpit data
     */
    void CContextOwnAircraft::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        if (this->getRuntime()->isSlotLogForOwnAircraftEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, com1.toQString(), com2.toQString(), transponder.toQString());
        bool changed = false;
        if (com1 != this->m_ownAircraft.getCom1System())
        {
            this->m_ownAircraft.setCom1System(com1);
            changed = true;
        }
        if (com2 != this->m_ownAircraft.getCom2System())
        {
            this->m_ownAircraft.setCom2System(com2);
            changed = true;
        }
        if (transponder != this->m_ownAircraft.getTransponder())
        {
            this->m_ownAircraft.setTransponder(transponder);
            changed = true;
        }

        if (!changed) return;
    }

    /*
     * Own aircraft
     */
    CAircraft CContextOwnAircraft::getOwnAircraft() const
    {
        if (this->getRuntime()->isSlotLogForOwnAircraftEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, this->m_ownAircraft.toQString());
        return this->m_ownAircraft;
    }

} // namespace
