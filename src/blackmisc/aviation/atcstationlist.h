/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ATCSTATIONLIST_H
#define BLACKMISC_AVIATION_ATCSTATIONLIST_H

#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QHash>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAtcStation, CAtcStationList)

namespace BlackMisc::Aviation
{
    //! Value object for a list of ATC stations.
    class BLACKMISC_EXPORT CAtcStationList :
        public CSequence<CAtcStation>,
        public Aviation::ICallsignObjectList<CAtcStation, CAtcStationList>,
        public Geo::IGeoObjectWithRelativePositionList<CAtcStation, CAtcStationList>,
        public Mixin::MetaType<CAtcStationList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAtcStationList)
        using CSequence::CSequence;

        //! Default constructor.
        CAtcStationList();

        //! Construct from a base class object.
        CAtcStationList(const CSequence<CAtcStation> &other);

        //! Find 0..n stations tuned in frequency of COM unit (with 25kHz channel spacing)
        CAtcStationList findIfComUnitTunedIn25KHz(const CComSystem &comUnit) const;

        //! Find 0..n stations tuned in frequency of COM unit (with channel spacing)
        CAtcStationList findIfComUnitTunedInChannelSpacing(const CComSystem &comUnit) const;

        //! Any stations tuned in frequency of COM unit (with channel spacing)
        bool hasComUnitTunedInChannelSpacing(const CComSystem &comUnit) const;

        //! Find 0..n stations within channel spacing
        CAtcStationList findIfFrequencyIsWithinSpacing(const PhysicalQuantities::CFrequency &frequency, CComSystem::ChannelSpacing spacing);

        //! Update if message changed
        bool updateIfMessageChanged(const CInformationMessage &im, const CCallsign &callsign, bool overrideWithNewer);

        //! Set online status
        int setOnline(const CCallsign &callsign, bool online);

        //! Find 0..n stations with valid COM frequency
        //! \sa CAtcStation::hasValidFrequency
        CAtcStationList stationsWithValidFrequency() const;

        //! All controllers (with valid data)
        Network::CUserList getControllers() const;

        //! Remove if marked outside of range
        int removeIfOutsideRange();

        //! Those in range
        CAtcStationList findInRange() const;

        //! Synchronize with ATC station representing booking information.
        //! Both sides (booking, online station) will be updated.
        //! \pre Can be used only if the stored data in this list are online ATC stations
        int synchronizeWithBookedStation(CAtcStation &bookedAtcStation);

        //! Sort by ATC suffix sort order and distance
        void sortByAtcSuffixSortOrderAndDistance();

        //! Sorted by ATC suffix sort order and distance
        CAtcStationList sortedByAtcSuffixSortOrderAndDistance() const;

        //! Split per suffix
        //! \remark sort can be disabled if already sorted
        QHash<QString, CAtcStationList> splitPerSuffix(bool sort = true) const;
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAtcStation>)

#endif //guard
