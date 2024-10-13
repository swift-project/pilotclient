// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfs.h"
#include "../fsxcommon/simconnectsymbols.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackCore;
using namespace BlackSimPlugin::FsxCommon;

namespace BlackSimPlugin::Msfs
{
    CSimulatorMsFs::CSimulatorMsFs(const CSimulatorPluginInfo &info,
                                   IOwnAircraftProvider *ownAircraftProvider,
                                   IRemoteAircraftProvider *remoteAircraftProvider,
                                   IClientProvider *clientProvider,
                                   QObject *parent) : CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent)
    {
        this->setDefaultModel(
            { "Airbus A320 Neo Asobo",
              CAircraftModel::TypeModelMatchingDefaultModel,
              "Airbus A320 default model",
              CAircraftIcaoCode("A320", "L2J") });
    }

    bool CSimulatorMsFs::connectTo()
    {
#ifdef Q_OS_WIN64
        if (!loadAndResolveMSFSimConnect())
        {
            return false;
        }
        return CSimulatorFsxCommon::connectTo();
#else
        if (!loadAndResolveFsxSimConnect(true))
        {
            return false;
        }
        return CSimulatorFsxCommon::connectTo();
#endif
    }

    void CSimulatorMsFs::setTrueAltitude(CAircraftSituation &aircraftSituation, const DataDefinitionOwnAircraft &simulatorOwnAircraft)
    {
        aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitudeCalibratedFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
        m_altitudeDelta = { simulatorOwnAircraft.altitudeFt - simulatorOwnAircraft.altitudeCalibratedFt, CLengthUnit::ft() };
    }

    void CSimulatorMsFsListener::startImpl()
    {
#ifdef Q_OS_WIN64
        if (!loadAndResolveMSFSimConnect())
        {
            return;
        }
        CSimulatorFsxCommonListener::startImpl();
#else
        if (!loadAndResolveFsxSimConnect(true))
        {
            return;
        }
        CSimulatorFsxCommonListener::startImpl();
#endif
    }

} // ns
