// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_FLIGHTPLANLIST_H
#define SWIFT_MISC_AVIATION_FLIGHTPLANLIST_H

#include <QMetaType>

#include "misc/aviation/callsignobjectlist.h"
#include "misc/aviation/flightplan.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CFlightPlan, CFlightPlanList)

namespace swift::misc::aviation
{
    //! Value object for a list of flight plans.
    class SWIFT_MISC_EXPORT CFlightPlanList :
        public CSequence<CFlightPlan>,
        public ICallsignObjectList<CFlightPlan, CFlightPlanList>,
        public mixin::MetaType<CFlightPlanList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CFlightPlanList)
        using CSequence::CSequence;

        //! Default constructor.
        CFlightPlanList();

        //! Construct from a base class object.
        CFlightPlanList(const CSequence<CFlightPlan> &other);
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CFlightPlanList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CFlightPlan>)

#endif // guard
