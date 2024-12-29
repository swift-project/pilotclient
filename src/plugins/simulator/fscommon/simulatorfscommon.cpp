// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfscommon.h"

#include "core/webdataservices.h"
#include "misc/logmessage.h"
#include "misc/simplecommandparser.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::core;
using namespace swift::simplugin::common;

namespace swift::simplugin::fscommon
{
    CSimulatorFsCommon::CSimulatorFsCommon(const CSimulatorPluginInfo &info, IOwnAircraftProvider *ownAircraftProvider,
                                           IRemoteAircraftProvider *renderedAircraftProvider,
                                           IClientProvider *clientProvider, QObject *parent)
        : CSimulatorPluginCommon(info, ownAircraftProvider, renderedAircraftProvider, clientProvider, parent)
    {
        CSimulatorFsCommon::registerHelp();
    }

    CSimulatorFsCommon::~CSimulatorFsCommon() {}

    void CSimulatorFsCommon::initSimulatorInternals()
    {
        CSimulatorPluginCommon::initSimulatorInternals();
        m_simulatorInternals.setSimulatorVersion(this->getSimulatorVersion());
    }

    void CSimulatorFsCommon::reset()
    {
        m_ownAircraftUpdateCycles = 0;
        m_skipCockpitUpdateCycles = 0;

        ISimulator::reset();
    }

    void CSimulatorFsCommon::registerHelp()
    {
        if (CSimpleCommandParser::registered("swift::simplugin::fscommon::CSimulatorFsCommon")) { return; }
        CSimpleCommandParser::registerCommand({ ".drv", "alias: .driver .plugin" });
    }

    bool CSimulatorFsCommon::disconnectFrom()
    {
        // reset flags
        m_simPaused = false;
        const bool r = CSimulatorPluginCommon::disconnectFrom();
        this->emitSimulatorCombinedStatus();
        return r;
    }
} // namespace swift::simplugin::fscommon
