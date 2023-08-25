// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORINFOLIST_H
#define BLACKMISC_SIMULATION_SIMULATORINFOLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CSimulatorInfo, CSimulatorPluginInfoList)

namespace BlackMisc::Simulation
{
    //! Value object encapsulating a list of distributors.
    class BLACKMISC_EXPORT CSimulatorInfoList :
        public BlackMisc::CSequence<CSimulatorInfo>,
        public BlackMisc::Mixin::MetaType<CSimulatorInfoList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSimulatorInfoList)
        using CSequence::CSequence;

        //! Default constructor.
        CSimulatorInfoList();

        //! Construct from a base class object.
        CSimulatorInfoList(const CSequence<CSimulatorInfo> &other);

        //! Find by id or alias
        CSimulatorInfoList withNoDuplicates() const;

        //! Split into single simulators, unknown simulators are ignored
        CSimulatorInfoList splitIntoSingleSimulators() const;

        //! Split into single simulators, unknown simulators are ignored
        static CSimulatorInfoList splitIntoSingleSimulators(const CSimulatorInfo &sim);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatorInfo>)

#endif // guard
