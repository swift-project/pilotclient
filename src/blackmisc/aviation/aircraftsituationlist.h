/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft situations
        class CAircraftSituationList :
            public CSequence<CAircraftSituation>,
            public ITimestampObjectList<CAircraftSituation, CAircraftSituationList>,
            public ICallsignObjectList<CAircraftSituation, CAircraftSituationList>
        {
        public:
            //! Default constructor.
            CAircraftSituationList();

            //! Construct from a base class object.
            CAircraftSituationList(const CSequence<CAircraftSituation> &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Metadata
            static void registerMetadata();

        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftSituation>)

#endif // guard
