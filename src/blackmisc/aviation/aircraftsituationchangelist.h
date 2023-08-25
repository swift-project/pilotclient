// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGELIST_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGELIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/timestampobjectlist.h"

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftSituationChange, CAircraftSituationChangeList)

namespace BlackMisc::Aviation
{
    //! Value object encapsulating a list of aircraft parts.
    class BLACKMISC_EXPORT CAircraftSituationChangeList :
        public CSequence<CAircraftSituationChange>,
        public ITimestampWithOffsetObjectList<CAircraftSituationChange, CAircraftSituationChangeList>,
        public Mixin::MetaType<CAircraftSituationChangeList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationChangeList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftSituationChangeList();

        //! Construct from a base class object.
        CAircraftSituationChangeList(const CSequence<CAircraftSituationChange> &other);

        //! Front or NULL
        CAircraftSituationChange frontOrNull() const;

        //! Back or NULL
        CAircraftSituationChange backOrNull() const;

        //! Index or NULL
        CAircraftSituationChange indexOrNull(int index) const;
    };

    //! Change per callsign
    using CAircraftSituationChangePerCallsign = QHash<CCallsign, CAircraftSituationChange>;

    //! Changes per callsign
    using CAircraftSituationChangeListPerCallsign = QHash<CCallsign, CAircraftSituationChangeList>;

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChangeList)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChangePerCallsign)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChangeListPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftSituationChange>)

#endif // guard
