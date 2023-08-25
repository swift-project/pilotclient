// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_FLIGHTPLANLIST_H
#define BLACKMISC_AVIATION_FLIGHTPLANLIST_H

#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Aviation, CFlightPlan, CFlightPlanList)

namespace BlackMisc::Aviation
{
    //! Value object for a list of flight plans.
    class BLACKMISC_EXPORT CFlightPlanList :
        public CSequence<CFlightPlan>,
        public ICallsignObjectList<CFlightPlan, CFlightPlanList>,
        public Mixin::MetaType<CFlightPlanList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CFlightPlanList)
        using CSequence::CSequence;

        //! Default constructor.
        CFlightPlanList();

        //! Construct from a base class object.
        CFlightPlanList(const CSequence<CFlightPlan> &other);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlanList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CFlightPlan>)

#endif // guard
