/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/timestampobjectlist.h"
#include <QHash>
#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft parts.
        class BLACKMISC_EXPORT CAircraftPartsList :
            public CSequence<CAircraftParts>,
            public ITimestampWithOffsetObjectList<CAircraftParts, CAircraftPartsList>,
            public Mixin::MetaType<CAircraftPartsList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftPartsList)
            using CSequence::CSequence;

            //! Default constructor.
            CAircraftPartsList();

            //! Construct from a base class object.
            CAircraftPartsList(const CSequence<CAircraftParts> &other);

            //! Set on ground for all entries
            int setOnGround(bool onGround);
        };

        //! Parts per callsign
        using CAircraftPartsPerCallsign = QHash<CCallsign, CAircraftParts>;

        //! Parts (list) per callsign
        using CAircraftPartsListPerCallsign = QHash<CCallsign, CAircraftPartsList>;

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftPartsList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftParts>)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftPartsPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftPartsListPerCallsign)

#endif //guard
