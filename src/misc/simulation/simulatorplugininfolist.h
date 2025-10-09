// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATORPLUGININFOLIST_H
#define SWIFT_MISC_SIMULATION_SIMULATORPLUGININFOLIST_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation, CSimulatorPluginInfo, CSimulatorPluginInfoList)

namespace swift::misc::simulation
{
    class CSimulatorPluginInfo;

    //! Value object encapsulating a list of SimulatorInfo objects.
    class SWIFT_MISC_EXPORT CSimulatorPluginInfoList :
        public swift::misc::CSequence<CSimulatorPluginInfo>,
        public swift::misc::mixin::MetaType<CSimulatorPluginInfoList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CSimulatorPluginInfoList)
        using CSequence::CSequence;

        //! Default constructor
        CSimulatorPluginInfoList() = default;

        //! Construct from a base class object.
        CSimulatorPluginInfoList(const CSequence<CSimulatorPluginInfo> &other);

        //! Is simulator supported
        bool supportsSimulator(const QString &simulator) const;

        //! String list with meaningful representations
        QStringList toStringList(bool i18n = false) const;

        //! Find by identifier (unique)
        CSimulatorPluginInfo findByIdentifier(const QString &identifier) const;

        //! Find by simulator
        CSimulatorPluginInfo findBySimulator(const swift::misc::simulation::CSimulatorInfo &simulator) const;
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorPluginInfoList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CSimulatorPluginInfo>)

#endif // SWIFT_MISC_SIMULATION_SIMULATORPLUGININFOLIST_H
