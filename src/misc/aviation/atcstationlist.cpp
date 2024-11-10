// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/mixin/mixincompare.h"
#include "misc/predicates.h"
#include "misc/range.h"

#include <QString>
#include <QtGlobal>

using namespace swift::misc::physical_quantities;
using namespace swift::misc::network;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CAtcStation, CAtcStationList)

namespace swift::misc::aviation
{
    CAtcStationList::CAtcStationList() {}

    CAtcStationList::CAtcStationList(const CSequence<CAtcStation> &other) : CSequence<CAtcStation>(other)
    {}

    CAtcStationList CAtcStationList::findIfComUnitTunedInChannelSpacing(const CComSystem &comUnit) const
    {
        return this->findBy([&](const CAtcStation &atcStation) {
            return atcStation.isComUnitTunedToFrequency(comUnit);
        });
    }

    bool CAtcStationList::hasComUnitTunedInChannelSpacing(const CComSystem &comUnit) const
    {
        return this->containsBy([&](const CAtcStation &atcStation) {
            return atcStation.isComUnitTunedToFrequency(comUnit);
        });
    }

    CAtcStationList CAtcStationList::findIfFrequencyIsWithinSpacing(const CFrequency &frequency)
    {
        if (frequency.isNull()) { return CAtcStationList(); }
        return this->findBy([&](const CAtcStation &atcStation) {
            return atcStation.isAtcStationFrequency(frequency);
        });
    }

    bool CAtcStationList::updateIfMessageChanged(const CInformationMessage &im, const CCallsign &callsign, bool overrideWithNewer)
    {
        const CInformationMessage::InformationType type = im.getType();

        // for loop just to get reference
        bool unequal = false;
        for (CAtcStation &station : *this)
        {
            if (station.getCallsign() != callsign) { continue; }

            const CInformationMessage m = station.getInformationMessage(type);
            if (m.getType() == CInformationMessage::Unspecified) { break; }

            if (m.getMessage() == im.getMessage())
            {
                if (!overrideWithNewer) { break; }
                if (!im.isNewerThan(m)) { break; }
            }
            else
            {
                unequal = true;
            }
            station.setMessage(im);
            break; // only count unequals
        }
        return unequal;
    }

    int CAtcStationList::setOnline(const CCallsign &callsign, bool online)
    {
        int c = 0;
        for (CAtcStation &station : *this)
        {
            if (station.getCallsign() != callsign) { continue; }
            if (station.setOnline(online)) { c++; }
        }
        return c;
    }

    CAtcStationList CAtcStationList::stationsWithValidFrequency() const
    {
        return this->findBy(&CAtcStation::hasValidFrequency, true);
    }

    CUserList CAtcStationList::getControllers() const
    {
        return this->findBy(predicates::MemberValid(&CAtcStation::getController)).transform(predicates::MemberTransform(&CAtcStation::getController));
    }

    int CAtcStationList::removeIfOutsideRange()
    {
        return this->removeIf(&CAtcStation::isInRange, false);
    }

    CAtcStationList CAtcStationList::findInRange() const
    {
        if (this->isEmpty()) { return {}; }
        CAtcStationList copy(*this);
        copy.removeIfOutsideRange();
        return copy;
    }

    void CAtcStationList::sortByAtcSuffixSortOrderAndDistance()
    {
        this->sortBy(&CAtcStation::getSuffixSortOrder, &CAtcStation::getRelativeDistance);
    }

    CAtcStationList CAtcStationList::sortedByAtcSuffixSortOrderAndDistance() const
    {
        CAtcStationList stations = *this;
        stations.sortByAtcSuffixSortOrderAndDistance();
        return stations;
    }

    QHash<QString, CAtcStationList> CAtcStationList::splitPerSuffix(bool sort) const
    {
        if (this->isEmpty()) { return QHash<QString, CAtcStationList>(); }
        const CAtcStationList stations = sort ? this->sortedByAtcSuffixSortOrderAndDistance() : *this;

        QString suffix;
        QHash<QString, CAtcStationList> split;
        for (const CAtcStation &s : stations)
        {
            const QString currentSuffix = s.getCallsignSuffix();
            if (suffix != currentSuffix)
            {
                suffix = currentSuffix;
                split[currentSuffix] = CAtcStationList();
            }
            split[currentSuffix].push_back(s);
        }
        return split;
    }
} // namespace
