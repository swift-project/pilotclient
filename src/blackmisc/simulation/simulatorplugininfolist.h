/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORPLUGININFOLIST_H
#define BLACKMISC_SIMULATION_SIMULATORPLUGININFOLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CSimulatorPluginInfo, CSimulatorPluginInfoList)

namespace BlackMisc::Simulation
{
    class CSimulatorPluginInfo;

    //! Value object encapsulating a list of SimulatorInfo objects.
    class BLACKMISC_EXPORT CSimulatorPluginInfoList :
        public BlackMisc::CSequence<CSimulatorPluginInfo>,
        public BlackMisc::Mixin::MetaType<CSimulatorPluginInfoList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSimulatorPluginInfoList)
        using CSequence::CSequence;

        //! Default constructor
        CSimulatorPluginInfoList();

        //! Construct from a base class object.
        CSimulatorPluginInfoList(const CSequence<CSimulatorPluginInfo> &other);

        //! Is simulator supported
        bool supportsSimulator(const QString &simulator) const;

        //! String list with meaningful representations
        QStringList toStringList(bool i18n = false) const;

        //! Find by identifier (unique)
        CSimulatorPluginInfo findByIdentifier(const QString &identifier) const;

        //! Find by simulator
        CSimulatorPluginInfo findBySimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorPluginInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatorPluginInfo>)

#endif // guard
