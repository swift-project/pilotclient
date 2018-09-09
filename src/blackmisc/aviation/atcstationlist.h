/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ATCSTATIONLIST_H
#define BLACKMISC_AVIATION_ATCSTATIONLIST_H

#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        class CComSystem;

        //! Value object for a list of ATC stations.
        class BLACKMISC_EXPORT CAtcStationList :
            public CSequence<CAtcStation>,
            public Aviation::ICallsignObjectList<CAtcStation, CAtcStationList>,
            public Geo::IGeoObjectWithRelativePositionList<CAtcStation, CAtcStationList>,
            public Mixin::MetaType<CAtcStationList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAtcStationList)

            //! Default constructor.
            CAtcStationList();

            //! Construct from a base class object.
            CAtcStationList(const CSequence<CAtcStation> &other);

            //! Find 0..n stations tune in frequency of COM unit (with 25kHt channel spacing
            CAtcStationList findIfComUnitTunedIn25KHz(const CComSystem &comUnit) const;

            //! Update if message changed
            int updateIfMessageChanged(const CInformationMessage &im, bool overrideWithNewer);

            //! Set online status
            int setOnline(const CCallsign &callsign, bool online);

            //! Find 0..n stations with valid voice room
            //! \sa CAtcStation::hasValidVoiceRoom
            CAtcStationList stationsWithValidVoiceRoom() const;

            //! Find 0..n stations with valid COM frequency
            //! \sa CAtcStation::hasValidFrequency
            CAtcStationList stationsWithValidFrequency() const;

            //! All controllers (with valid data)
            BlackMisc::Network::CUserList getControllers() const;

            //! Remove if marked outside of range
            int removeIfOutsideRange();

            //! Synchronize with ATC station representing booking information.
            //! Both sides (booking, online station) will be updated.
            //! \pre Can be used only if the stored data in this list are online ATC stations
            int synchronizeWithBookedStation(CAtcStation &bookedAtcStation);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAtcStation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAtcStation>)

#endif //guard
