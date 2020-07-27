/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORINFOLIST_H
#define BLACKMISC_SIMULATION_SIMULATORINFOLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/variant.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Simulation
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatorInfo>)

#endif //guard
