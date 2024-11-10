// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONSETUPLIST_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONSETUPLIST_H

#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/aviation/callsignobjectlist.h"
#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/sequence.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation, CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList)

namespace swift::misc::simulation
{
    //! Value object encapsulating a list of setups.
    class SWIFT_MISC_EXPORT CInterpolationSetupList :
        public CSequence<CInterpolationAndRenderingSetupPerCallsign>,
        public aviation::ICallsignObjectList<CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList>,
        public mixin::MetaType<CInterpolationSetupList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CInterpolationSetupList)
        using CSequence::CSequence;

        //! Default constructor.
        CInterpolationSetupList();

        //! Single object
        CInterpolationSetupList(const CInterpolationAndRenderingSetupPerCallsign &setup);

        //! Construct from a base class object.
        CInterpolationSetupList(const CSequence<CInterpolationAndRenderingSetupPerCallsign> &other);
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::simulation::CInterpolationSetupList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign>)

#endif
