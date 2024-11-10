// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/user.h"
#include "misc/predicates.h"
#include "misc/range.h"

#include <QString>
#include <tuple>

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::network;

BLACK_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation, CSimulatedAircraft, CSimulatedAircraftList)

namespace swift::misc::simulation
{
    CSimulatedAircraftList::CSimulatedAircraftList() {}

    CSimulatedAircraftList::CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other) : CSequence<CSimulatedAircraft>(other)
    {}

    CUserList CSimulatedAircraftList::getPilots() const
    {
        return this->findBy(predicates::MemberValid(&CSimulatedAircraft::getPilot)).transform(predicates::MemberTransform(&CSimulatedAircraft::getPilot));
    }

    CAircraftModelList CSimulatedAircraftList::getModels() const
    {
        return this->transform(predicates::MemberTransform(&CSimulatedAircraft::getModel));
    }

    CSimulatedAircraftList CSimulatedAircraftList::findByEnabled(bool enabled) const
    {
        return this->findBy(&CSimulatedAircraft::isEnabled, enabled);
    }

    CSimulatedAircraftList CSimulatedAircraftList::findByRendered(bool rendered) const
    {
        return this->findBy(&CSimulatedAircraft::isRendered, rendered);
    }

    CSimulatedAircraftList CSimulatedAircraftList::findByVtol(bool vtol) const
    {
        return this->findBy(&CSimulatedAircraft::isVtol, vtol);
    }

    CCallsignSet CSimulatedAircraftList::getCallsignsWithSynchronizedParts() const
    {
        CCallsignSet csl;
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (!aircraft.isPartsSynchronized()) { continue; }
            csl.push_back(aircraft.getCallsign());
        }
        return csl;
    }

    bool CSimulatedAircraftList::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUpdated) const
    {
        if (this->isEmpty()) return false;
        if (aircraftToBeUpdated.hasRealName() && aircraftToBeUpdated.hasId() && aircraftToBeUpdated.hasAircraftAndAirlineDesignator()) { return false; }

        CSimulatedAircraft currentDataFileAircraft = this->findFirstByCallsign(aircraftToBeUpdated.getCallsign());
        if (currentDataFileAircraft.getCallsign().isEmpty()) return false;

        CUser user = aircraftToBeUpdated.getPilot();
        user.updateMissingParts(currentDataFileAircraft.getPilot());
        aircraftToBeUpdated.setPilot(user);

        CAircraftIcaoCode aircraftIcao = aircraftToBeUpdated.getAircraftIcaoCode();
        CAirlineIcaoCode airlineIcao = aircraftToBeUpdated.getAirlineIcaoCode();
        aircraftIcao.updateMissingParts(currentDataFileAircraft.getAircraftIcaoCode());
        airlineIcao.updateMissingParts(currentDataFileAircraft.getAirlineIcaoCode());
        aircraftToBeUpdated.setIcaoCodes(aircraftIcao, airlineIcao);
        return true;
    }

    void CSimulatedAircraftList::markAllAsNotRendered()
    {
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (!aircraft.isRendered()) { continue; }
            aircraft.setRendered(false);
        }
    }

    int CSimulatedAircraftList::setRendered(const CCallsign &callsign, bool rendered, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setRendered(rendered);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setCG(const CCallsign &callsign, const CLength &cg, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setCG(cg);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setFastPositionUpdates(const CCallsign &callsign, bool fastPositions, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setFastPositionUpdates(fastPositions);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setEnabled(const CCallsign &callsign, bool enabled, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setEnabled(enabled);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setAircraftModel(const CCallsign &callsign, const CAircraftModel &model, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setModel(model);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setAircraftPartsSynchronized(const CCallsign &callsign, const CAircraftParts &parts, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setParts(parts);
            aircraft.setPartsSynchronized(true);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setAircraftSituation(const CCallsign &callsign, const CAircraftSituation &situation, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setSituation(situation); // also sets setSupportingGndFlag
            if (onlyFirst) break;
        }
        return c;
    }

    int CSimulatedAircraftList::setGroundElevationChecked(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info, bool onlyFirst)
    {
        int c = 0;
        for (CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            aircraft.setGroundElevationChecked(elevation, info);
            c++;
            if (onlyFirst) break;
        }
        return c;
    }

    bool CSimulatedAircraftList::isEnabled(const CCallsign &callsign) const
    {
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            return aircraft.isEnabled();
        }
        return false;
    }

    bool CSimulatedAircraftList::isRendered(const CCallsign &callsign) const
    {
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.getCallsign() != callsign) { continue; }
            return aircraft.isRendered();
        }
        return false;
    }

    bool CSimulatedAircraftList::replaceOrAddByCallsign(const CSimulatedAircraft &aircraft)
    {
        const CCallsign cs(aircraft.getCallsign());
        if (cs.isEmpty()) { return false; }

        if (this->containsCallsign(cs))
        {
            int c = this->replaceIf(&CSimulatedAircraft::getCallsign, cs, aircraft);
            return c > 0;
        }
        this->push_back(aircraft);
        return true;
    }

    int CSimulatedAircraftList::countEnabled() const
    {
        int c = 0;
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.isEnabled()) { c++; }
        }
        return c;
    }

    int CSimulatedAircraftList::countRendered() const
    {
        int c = 0;
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.isRendered()) { c++; }
        }
        return c;
    }

    int CSimulatedAircraftList::countAircraftPartsSynchronized() const
    {
        int c = 0;
        for (const CSimulatedAircraft &aircraft : (*this))
        {
            if (aircraft.isPartsSynchronized()) { c++; }
        }
        return c;
    }

    void CSimulatedAircraftList::sortByDistanceToReferencePositionRenderedCallsign()
    {
        this->sort([&](const CSimulatedAircraft &a, const CSimulatedAircraft &b) {
            if (a.getRelativeDistance() != b.getRelativeDistance()) { return a.getRelativeDistance() < b.getRelativeDistance(); }
            if (a.isRendered() != b.isRendered()) { return a.isRendered(); } // get the rendered first
            return a.getCallsignAsString() < b.getCallsignAsString();
        });
    }
} // namespace
