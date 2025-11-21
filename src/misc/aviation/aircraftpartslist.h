// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTPARTSLIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTPARTSLIST_H

#include <QHash>
#include <QMetaType>

#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/callsign.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftParts, CAircraftPartsList)

namespace swift::misc::aviation
{
    //! Value object encapsulating a list of aircraft parts.
    class SWIFT_MISC_EXPORT CAircraftPartsList :
        public CSequence<CAircraftParts>,
        public ITimestampWithOffsetObjectList<CAircraftParts, CAircraftPartsList>,
        public mixin::MetaType<CAircraftPartsList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftPartsList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftPartsList() = default;

        //! Construct from a base class object.
        CAircraftPartsList(const CSequence<CAircraftParts> &other);

        //! Set on ground for all entries
        int setOnGround(bool onGround);
    };

    //! Parts per callsign
    using CAircraftPartsPerCallsign = QHash<CCallsign, CAircraftParts>;

    //! Parts (list) per callsign
    using CAircraftPartsListPerCallsign = QHash<CCallsign, CAircraftPartsList>;

} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftPartsList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftParts>)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftPartsPerCallsign)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftPartsListPerCallsign)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTPARTSLIST_H
