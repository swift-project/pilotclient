// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONSETUPLIST_H
#define BLACKMISC_SIMULATION_INTERPOLATIONSETUPLIST_H

#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList)

namespace BlackMisc::Simulation
{
    //! Value object encapsulating a list of setups.
    class BLACKMISC_EXPORT CInterpolationSetupList :
        public CSequence<CInterpolationAndRenderingSetupPerCallsign>,
        public Aviation::ICallsignObjectList<CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList>,
        public Mixin::MetaType<CInterpolationSetupList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CInterpolationSetupList)
        using CSequence::CSequence;

        //! Default constructor.
        CInterpolationSetupList();

        //! Single object
        CInterpolationSetupList(const CInterpolationAndRenderingSetupPerCallsign &setup);

        //! Construct from a base class object.
        CInterpolationSetupList(const CSequence<CInterpolationAndRenderingSetupPerCallsign> &other);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationSetupList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign>)

#endif // guard
