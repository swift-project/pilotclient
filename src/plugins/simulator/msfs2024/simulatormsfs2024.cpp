// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorMsFs2024.h"

#include "../fsxcommon/simconnectsymbols.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::core;
using namespace swift::simplugin::fsxcommon;

namespace swift::simplugin::msfs2024
{
    CSimulatorMsFs2024::CSimulatorMsFs2024(const CSimulatorPluginInfo &info, IOwnAircraftProvider *ownAircraftProvider,
                                           IRemoteAircraftProvider *remoteAircraftProvider,
                                           IClientProvider *clientProvider, QObject *parent)
        : CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent)
    {
        this->setDefaultModel({ "Airbus A320 Neo Asobo", CAircraftModel::TypeModelMatchingDefaultModel,
                                "Airbus A320 default model", CAircraftIcaoCode("A320", "L2J") });
    }

    bool CSimulatorMsFs2024::connectTo()
    {
        if (!loadAndResolveMSFS2024SimConnect()) { return false; }
        return CSimulatorFsxCommon::connectTo();
    }

    void CSimulatorMsFs2024::setTrueAltitude(CAircraftSituation &aircraftSituation,
                                             const DataDefinitionOwnAircraft &simulatorOwnAircraft)
    {
        aircraftSituation.setAltitude(
            CAltitude(simulatorOwnAircraft.altitudeCalibratedFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
        m_altitudeDelta = { simulatorOwnAircraft.altitudeFt - simulatorOwnAircraft.altitudeCalibratedFt,
                            CLengthUnit::ft() };
    }

    void CSimulatorMsFs2024Listener::startImpl()
    {
        if (!loadAndResolveMSFS2024SimConnect()) { return; }
        CSimulatorFsxCommonListener::startImpl();
    }

} // namespace swift::simplugin::msfs2024
