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
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/variant.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Geo { class CElevationPlane; }
    namespace Aviation
    {
        class CAircraftSituation;

        //! Value object encapsulating a list of aircraft situations
        class BLACKMISC_EXPORT CAircraftSituationList :
            public CSequence<CAircraftSituation>,
            public ITimestampWithOffsetObjectList<CAircraftSituation, CAircraftSituationList>,
            public ICallsignObjectList<CAircraftSituation, CAircraftSituationList>,
            public Mixin::MetaType<CAircraftSituationList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationList)

            //! Default constructor.
            CAircraftSituationList();

            //! Construct from a base class object.
            CAircraftSituationList(const CSequence<CAircraftSituation> &other);

            //! Construct from initializer list.
            CAircraftSituationList(std::initializer_list<CAircraftSituation> il);

            //! Set ground elevation from elevation plane
            int setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, bool ignoreNullValues = true, bool overrideExisting = true);

            //! Set ground elevation from elevation plane
            int setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, qint64 newerThanAdjustedMs, bool ignoreNullValues = true, bool overrideExisting = true);
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftSituation>)

#endif // guard
