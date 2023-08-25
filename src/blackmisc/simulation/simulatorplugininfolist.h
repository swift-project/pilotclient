// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
