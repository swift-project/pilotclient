// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATORINFOLIST_H
#define SWIFT_MISC_SIMULATION_SIMULATORINFOLIST_H

#include <QMetaType>

#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::Simulation, CSimulatorInfo, CSimulatorPluginInfoList)

namespace swift::misc::simulation
{
    //! Value object encapsulating a list of distributors.
    class SWIFT_MISC_EXPORT CSimulatorInfoList :
        public swift::misc::CSequence<CSimulatorInfo>,
        public swift::misc::mixin::MetaType<CSimulatorInfoList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CSimulatorInfoList)
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
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorInfoList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CSimulatorInfo>)

#endif // guard
