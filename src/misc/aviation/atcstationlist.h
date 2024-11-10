// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_ATCSTATIONLIST_H
#define SWIFT_MISC_AVIATION_ATCSTATIONLIST_H

#include "misc/aviation/atcstation.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/callsignobjectlist.h"
#include "misc/pq/frequency.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/network/userlist.h"
#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/sequence.h"

#include <QMetaType>
#include <QHash>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAtcStation, CAtcStationList)

namespace swift::misc::aviation
{
    //! Value object for a list of ATC stations.
    class SWIFT_MISC_EXPORT CAtcStationList :
        public CSequence<CAtcStation>,
        public aviation::ICallsignObjectList<CAtcStation, CAtcStationList>,
        public geo::IGeoObjectWithRelativePositionList<CAtcStation, CAtcStationList>,
        public mixin::MetaType<CAtcStationList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAtcStationList)
        using CSequence::CSequence;

        //! Default constructor.
        CAtcStationList();

        //! Construct from a base class object.
        CAtcStationList(const CSequence<CAtcStation> &other);

        //! Find 0..n stations tuned in frequency of COM unit (with channel spacing)
        CAtcStationList findIfComUnitTunedInChannelSpacing(const CComSystem &comUnit) const;

        //! Any stations tuned in frequency of COM unit (with channel spacing)
        bool hasComUnitTunedInChannelSpacing(const CComSystem &comUnit) const;

        //! Find 0..n stations with frequency (with 5 kHz spacing for .x20/.x25 and .x70/.x75)
        CAtcStationList findIfFrequencyIsWithinSpacing(const physical_quantities::CFrequency &frequency);

        //! Update if message changed
        bool updateIfMessageChanged(const CInformationMessage &im, const CCallsign &callsign, bool overrideWithNewer);

        //! Set online status
        int setOnline(const CCallsign &callsign, bool online);

        //! Find 0..n stations with valid COM frequency
        //! \sa CAtcStation::hasValidFrequency
        CAtcStationList stationsWithValidFrequency() const;

        //! All controllers (with valid data)
        network::CUserList getControllers() const;

        //! Remove if marked outside of range
        int removeIfOutsideRange();

        //! Those in range
        CAtcStationList findInRange() const;

        //! Sort by ATC suffix sort order and distance
        void sortByAtcSuffixSortOrderAndDistance();

        //! Sorted by ATC suffix sort order and distance
        CAtcStationList sortedByAtcSuffixSortOrderAndDistance() const;

        //! Split per suffix
        //! \remark sort can be disabled if already sorted
        QHash<QString, CAtcStationList> splitPerSuffix(bool sort = true) const;
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::aviation::CAtcStationList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAtcStation>)

#endif // guard
