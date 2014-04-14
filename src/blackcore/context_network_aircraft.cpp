/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "context_runtime.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"

#include "blackmisc/predicates.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    /*
     * Aircraft info received
     */
    void CContextNetwork::psFsdIcaoCodesReceived(const CCallsign &callsign, const CAircraftIcao &icaoData)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), icaoData.toQString());
        CAircraftList aircraftsWithCallsign = this->m_aircraftsInRange.findByCallsign(callsign);
        if (aircraftsWithCallsign.isEmpty())
        {
            // new aircraft
            CAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setIcaoInfo(icaoData);
            aircraft.calculcateDistanceToPlane(this->m_ownAircraft.getPosition());
            this->m_aircraftsInRange.push_back(aircraft);
            if (this->isConnected())
            {
                // emit only if still connected
                emit this->m_network->sendFrequencyQuery(callsign);
                emit this->m_network->sendRealNameQuery(callsign);
            }
            emit this->changedAircraftsInRange();
        }
        else
        {
            // update
            CIndexVariantMap vm(CAircraft::IndexIcao, icaoData.toQVariant());
            this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual<CAircraft>(&CAircraft::getCallsign, callsign), vm);
            emit this->changedAircraftsInRange();
        }
    }

    /*
     * Aircraft update received
     */
    void CContextNetwork::psFsdAircraftUpdateReceived(const CCallsign &callsign, const CAircraftSituation &situation, const CTransponder &transponder)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), situation.toQString(), transponder.toQString());

        CAircraftList list = this->m_aircraftsInRange.findByCallsign(callsign);
        if (list.isEmpty())
        {
            // new aircraft
            CAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            aircraft.calculcateDistanceToPlane(this->m_ownAircraft.getPosition());
            this->m_vatsimDataFileReader->getAircrafts().updateFromVatsimDataFileAircraft(aircraft);
            this->m_aircraftsInRange.push_back(aircraft);

            if (this->isConnected())
            {
                // only emit if still connected
                emit this->m_network->sendFrequencyQuery(callsign);
                emit this->m_network->sendRealNameQuery(callsign);
                emit this->m_network->sendIcaoCodesQuery(callsign);
            }
        }
        else
        {
            // update
            CLength distance = this->m_ownAircraft.calculcateDistanceToPlane(situation.getPosition());
            distance.switchUnit(CLengthUnit::NM());
            CIndexVariantMap vm;
            vm.addValue(CAircraft::IndexTransponder, transponder);
            vm.addValue(CAircraft::IndexSituation, situation);
            vm.addValue(CAircraft::IndexDistance, distance);
            this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual<CAircraft>(&CAircraft::getCallsign, callsign), vm);
        }

        emit this->changedAircraftsInRange();
        emit changedAircraftSituation(callsign, situation);
    }

    /*
     * Pilot disconnected
     */
    void CContextNetwork::psFsdPilotDisconnected(const CCallsign &callsign)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString());
        this->m_aircraftsInRange.removeIf(&CAircraft::getCallsign, callsign);
    }

    /*
     * Frequency received
     */
    void CContextNetwork::psFsdFrequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), frequency.toQString());

        // update
        CIndexVariantMap vm(CAircraft::IndexFrequencyCom1, frequency.toQVariant());
        this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual<CAircraft>(&CAircraft::getCallsign, callsign), vm);
        emit this->changedAircraftsInRange();
    }

} // namespace
