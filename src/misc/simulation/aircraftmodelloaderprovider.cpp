// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/aircraftmodelloaderprovider.h"

#include <QDir>
#include <QMap>
#include <Qt>
#include <QtGlobal>

#include "misc/directoryutils.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/simulation/flightgear/aircraftmodelloaderflightgear.h"
#include "misc/simulation/fscommon/aircraftcfgparser.h"
#include "misc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "misc/simulation/xplane/xplaneutil.h"

using namespace swift::misc;
using namespace swift::misc::simulation::data;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::flightgear;
using namespace swift::misc::simulation::xplane;

namespace swift::misc::simulation
{
    IAircraftModelLoader *CMultiAircraftModelLoaderProvider::createModelLoader(const CSimulatorInfo &simulator,
                                                                               QObject *parent)
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
        case CSimulatorInfo::MSFS:
        {
            if (!m_loaderMsfs) { m_loaderMsfs = this->initLoader(CSimulatorInfo::msfs()); }
            return m_loaderMsfs;
        }
        case CSimulatorInfo::MSFS2024:
        {
            if (!m_loaderMsfs2024) { m_loaderMsfs2024 = this->initLoader(CSimulatorInfo::msfs2024()); }
            return m_loaderMsfs2024;
        }
        default: Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator"); break;
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
        bool c = connect(loader, &IAircraftModelLoader::loadingFinished, this,
                         &CMultiAircraftModelLoaderProvider::loadingFinished, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::diskLoadingStarted, this,
                    &CMultiAircraftModelLoaderProvider::diskLoadingStarted, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::cacheChanged, this, &CMultiAircraftModelLoaderProvider::cacheChanged,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(loader, &IAircraftModelLoader::loadingProgress, this,
                    &CMultiAircraftModelLoaderProvider::loadingProgress, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        return loader;
    }
} // namespace swift::misc::simulation
