/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTPARTSLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/variant.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft parts.
        class BLACKMISC_EXPORT CAircraftPartsList :
            public CSequence<CAircraftParts>,
            public ITimestampObjectList<CAircraftParts, CAircraftPartsList>,
            public BlackMisc::Mixin::MetaType<CAircraftPartsList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftPartsList)

            //! Default constructor.
            CAircraftPartsList();

            //! Construct from a base class object.
            CAircraftPartsList(const CSequence<CAircraftParts> &other);

            //! Construct from initializer list.
            CAircraftPartsList(std::initializer_list<CAircraftParts> il);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftPartsList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftParts>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftParts>)

#endif //guard
