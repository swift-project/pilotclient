// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/aircraftmodelloader.h"
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

namespace BlackMisc::Simulation
{
    const QStringList &IAircraftModelLoader::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::modelLoader() });
        return cats;
    }

    const QString &IAircraftModelLoader::enumToString(IAircraftModelLoader::LoadFinishedInfo info)
    {
        static const QString loaded("cache loaded");
        static const QString skipped("loading skipped");
        static const QString parsed("parsed data");
        static const QString failed("failed");

        switch (info)
        {
        case CacheLoaded: return loaded;
        case ParsedData: return parsed;
        case LoadingSkipped: return skipped;
        case LoadingFailed: return failed;
        default: break;
        }

        static const QString unknown("??");
        return unknown;
    }

    const QString &IAircraftModelLoader::enumToString(IAircraftModelLoader::LoadModeFlag modeFlag)
    {
        static const QString notSet("not set");
        static const QString directly("load directly");
        static const QString background("load in background");
        static const QString cacheFirst("cache first");
        static const QString cacheSkipped("cache skipped");
        static const QString cacheOnly("cacheOnly");

        switch (modeFlag)
        {
        case NotSet: return notSet;
        case LoadDirectly: return directly;
        case LoadInBackground: return background;
        case CacheFirst: return cacheFirst;
        case CacheSkipped: return cacheSkipped;
        case CacheOnly: return cacheOnly;
        default: break;
        }

        static const QString unknown("??");
        return unknown;
    }

    QString IAircraftModelLoader::enumToString(LoadMode mode)
    {
        QStringList modes;
        if (mode.testFlag(NotSet)) { modes << enumToString(NotSet); }
        if (mode.testFlag(LoadDirectly)) { modes << enumToString(LoadDirectly); }
        if (mode.testFlag(LoadInBackground)) { modes << enumToString(LoadInBackground); }
        if (mode.testFlag(CacheFirst)) { modes << enumToString(CacheFirst); }
        if (mode.testFlag(CacheSkipped)) { modes << enumToString(CacheSkipped); }
        return modes.join(", ");
    }

    bool IAircraftModelLoader::needsCacheSynchronized(LoadMode mode)
    {
        return mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly);
    }

    IAircraftModelLoader::IAircraftModelLoader(const CSimulatorInfo &simulator, QObject *parent) : QObject(parent),
                                                                                                   m_simulator(simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
        connect(this, &IAircraftModelLoader::loadingFinished, this, &IAircraftModelLoader::onLoadingFinished, Qt::QueuedConnection);

        CCentralMultiSimulatorModelCachesProvider *centralCaches = &CCentralMultiSimulatorModelCachesProvider::modelCachesInstance();
        connect(centralCaches, &CCentralMultiSimulatorModelCachesProvider::cacheChanged, this, &IAircraftModelLoader::onCacheChanged, Qt::QueuedConnection);
        this->setObjectInfo(simulator);
    }

    IAircraftModelLoader::~IAircraftModelLoader() {}

    void IAircraftModelLoader::startLoading(LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
    {
        if (m_loadingInProgress) { return; }
        if (mode == NotSet) { return; }
        m_loadingInProgress = true;
        m_loadingMessages.clear();

        const CSimulatorInfo simulator = this->getSimulator();
        const bool needsCacheSynced = IAircraftModelLoader::needsCacheSynchronized(mode);
        if (needsCacheSynced) { this->synchronizeCache(simulator); }

        const bool useCachedData = !mode.testFlag(CacheSkipped) && this->hasCachedData();
        if (useCachedData && (mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly)))
        {
            // we just just cache data
            static const CStatusMessage status(this, CStatusMessage::SeverityInfo, u"Using cached data");
            emit this->loadingFinished(status, simulator, CacheLoaded);
            return;
        }
        if (mode.testFlag(CacheOnly))
        {
            // only cache, but we did not find any data yet (still in progress?)
            // here we rely on the cache load slot, no need to emit here, will
            // be done later in ps_cacheChanged. An alternative was to sync cache here
            m_loadingInProgress = false;
            return;
        }

        // really load from disk?
        const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
        if (m_skipLoadingEmptyModelDir && modelDirs.isEmpty())
        {
            const CStatusMessage status = CStatusMessage(this, CStatusMessage::SeverityWarning,
                                                         u"Empty or not existing '%1' directory '%2', skipping read")
                                          << simulator.toQString() << modelDirectories.join(", ");
            m_loadingMessages.push_back(status);
            m_loadingMessages.freezeOrder();
            emit this->loadingFinished(m_loadingMessages, simulator, LoadingSkipped);
            return;
        }

        this->setObjectInfo(simulator);
        this->startLoadingFromDisk(mode, modelConsolidation, modelDirs);
    }

    QString IAircraftModelLoader::getFirstModelDirectoryOrDefault() const
    {
        const QString md = m_settings.getFirstModelDirectoryOrDefault(m_simulator);
        return md;
    }

    void IAircraftModelLoader::setModels(const CAircraftModelList &models)
    {
        this->setModelsForSimulator(models, m_simulator);
    }

    int IAircraftModelLoader::updateModels(const CAircraftModelList &models)
    {
        return this->updateModelsForSimulator(models, m_simulator);
    }

    QStringList IAircraftModelLoader::getInitializedModelDirectories(const QStringList &modelDirectories, const CSimulatorInfo &simulator) const
    {
        QStringList modelDirs = modelDirectories.isEmpty() ? m_settings.getModelDirectoriesOrDefault(simulator) : modelDirectories;
        modelDirs = CFileUtils::fixWindowsUncPaths(modelDirs);
        return CDirectoryUtils::getExistingUnemptyDirectories(modelDirs);
    }

    bool IAircraftModelLoader::hasCachedData() const
    {
        return !this->getCachedModels(m_simulator).isEmpty();
    }

    void IAircraftModelLoader::setObjectInfo(const CSimulatorInfo &simulatorInfo)
    {
        this->setObjectName("Model loader for: '" + simulatorInfo.toQString(true) + "'");
    }

    void IAircraftModelLoader::onLoadingFinished(const CStatusMessageList &statusMsgs, const CSimulatorInfo &simulator, IAircraftModelLoader::LoadFinishedInfo info)
    {
        if (!this->supportsSimulator(simulator)) { return; } // none of my business
        this->setObjectInfo(simulator);

        // remark: in the past status used to be bool, now it is CStatusMessage
        // so there is some redundancy here between status and m_loadingMessages
        m_loadingInProgress = false;

        const QMap<int, int> counts = statusMsgs.countSeverities();
        const int errors = counts.value(SeverityError);
        const int warnings = counts.value(SeverityWarning);

        if (statusMsgs.hasWarningOrErrorMessages())
        {
            CLogMessage(this).log(m_loadingMessages.worstSeverity(),
                                  u"Message loading produced %1 error and %2 warning messages")
                << errors << warnings;
        }
        else
        {
            CLogMessage(this).info(u"Loading '%1' finished, success for '%2'") << IAircraftModelLoader::enumToString(info) << simulator.toQString();
        }
    }

    void IAircraftModelLoader::onCacheChanged(const CSimulatorInfo &simulator)
    {
        if (m_loadingInProgress) { return; } // this change signal is redundant as it will be handled by onLoadingFinished
        if (!this->supportsSimulator(simulator)) { return; } // none of my business
        emit this->cacheChanged(simulator);
    }

    CDummyModelLoader::CDummyModelLoader(const CSimulatorInfo &simulator, QObject *parent) : IAircraftModelLoader(simulator, parent)
    {
        // void
    }

    bool CDummyModelLoader::isLoadingFinished() const
    {
        // fake loading
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        return m_loadingStartedTs > 0 && now > (m_loadingStartedTs + 5000);
    }

    void CDummyModelLoader::startLoadingFromDisk(LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
    {
        Q_UNUSED(mode);
        Q_UNUSED(modelConsolidation);
        Q_UNUSED(modelDirectories);
        m_loadingStartedTs = QDateTime::currentMSecsSinceEpoch();
    }

} // ns
