// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONCHANGELIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONCHANGELIST_H

#include "misc/aviation/aircraftsituationchange.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftSituationChange, CAircraftSituationChangeList)

namespace swift::misc::aviation
{
    //! Value object encapsulating a list of aircraft parts.
    class SWIFT_MISC_EXPORT CAircraftSituationChangeList :
        public CSequence<CAircraftSituationChange>,
        public ITimestampWithOffsetObjectList<CAircraftSituationChange, CAircraftSituationChangeList>,
        public mixin::MetaType<CAircraftSituationChangeList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationChangeList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftSituationChangeList() = default;

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

} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationChangeList)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationChangePerCallsign)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationChangeListPerCallsign)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftSituationChange>)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONCHANGELIST_H
