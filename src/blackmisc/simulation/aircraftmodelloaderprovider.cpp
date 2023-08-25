// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/aircraftmodelloaderprovider.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/flightgear/aircraftmodelloaderflightgear.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/logmessage.h"

#include <QDir>
#include <Qt>
#include <QtGlobal>
#include <QMap>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Flightgear;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackMisc::Simulation
{
    IAircraftModelLoader *CMultiAircraftModelLoaderProvider::createModelLoader(const CSimulatorInfo &simulator, QObject *parent)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Single simulator");
        if (simulator.isXPlane()) { return new CAircraftModelLoaderXPlane(parent); }
        if (simulator.isFG()) { return new CAircraftModelLoaderFlightgear(parent); }
        return CAircraftCfgParser::createModelLoader(simulator, parent);
    }

    IAircraftModelLoader *CMultiAircraftModelLoaderProvider::loaderInstance(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FSX:
        {
            if (!m_loaderFsx) { m_loaderFsx = this->initLoader(CSimulatorInfo::fsx()); }
            return m_loaderFsx;
        }
        case CSimulatorInfo::P3D:
        {
            if (!m_loaderP3D) { m_loaderP3D = this->initLoader(CSimulatorInfo::p3d()); }
            return m_loaderP3D;
        }
        case CSimulatorInfo::XPLANE:
        {
            if (!m_loaderXP) { m_loaderXP = this->initLoader(CSimulatorInfo::xplane()); }
            return m_loaderXP;
        }
        case CSimulatorInfo::FS9:
        {
            if (!m_loaderFS9) { m_loaderFS9 = this->initLoader(CSimulatorInfo::fs9()); }
            return m_loaderFS9;
        }
        case CSimulatorInfo::FG:
        {
            if (!m_loaderFG) { m_loaderFG = this->initLoader(CSimulatorInfo::fg()); }
            return m_loaderFG;
        }
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator");
            break;
        }
        return nullptr;
    }

    CMultiAircraftModelLoaderProvider &CMultiAircraftModelLoaderProvider::multiModelLoaderInstance()
    {
        static CMultiAircraftModelLoaderProvider loader;
        return loader;
    }

    IAircraftModelLoader *CMultiAircraftModelLoaderProvider::initLoader(const CSimulatorInfo &simulator)
    {
        // in some cases the loading progress signal was not send properly
        // changing to Qt::QueuedConnection has solved the issues (Ref T529)
        IAircraftModelLoader *loader = createModelLoader(simulator, this);
        bool c = connect(loader, &IAircraftModelLoader::loadingFinished, this, &CMultiAircraftModelLoaderProvider::loadingFinished, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::diskLoadingStarted, this, &CMultiAircraftModelLoaderProvider::diskLoadingStarted, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::cacheChanged, this, &CMultiAircraftModelLoaderProvider::cacheChanged, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::loadingProgress, this, &CMultiAircraftModelLoaderProvider::loadingProgress, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        return loader;
    }
} // ns
