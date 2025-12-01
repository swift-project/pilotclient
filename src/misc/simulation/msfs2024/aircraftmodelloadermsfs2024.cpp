// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

// #include <QMessageBox>

#include "misc/simulation/msfs2024/aircraftmodelloadermsfs2024.h"

#include <QDirIterator>

#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/settings/simulatorsettings.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;
// using namespace swift::misc::math;
using namespace swift::misc::simulation::data;
// using namespace swift::misc::simulation::msfs2024;
// using namespace swift::misc::simulation::settings;

namespace swift::misc::simulation::msfs2024
{

    bool CAircraftModelLoaderMsfs2024::isLoadingFinished() const
    {
        return !m_parserWorker || m_parserWorker->isFinished();
    }

    CAircraftModelLoaderMsfs2024::CAircraftModelLoaderMsfs2024(QObject *parent)
        : simulation::IAircraftModelLoader(simulation::CSimulatorInfo::msfs2024(), parent)
    {}

    CAircraftModelLoaderMsfs2024::~CAircraftModelLoaderMsfs2024()
    {
        // that should be safe as long as the worker uses deleteLater (which it does)
        if (m_parserWorker) { m_parserWorker->waitForFinished(); }
    }

    void CAircraftModelLoaderMsfs2024::updateInstalledModels(const CAircraftModelList &models)
    {
        this->setModelsForSimulator(models, CSimulatorInfo::msfs2024());

        const CStatusMessage m =
            CStatusMessage(this, CStatusMessage::SeverityInfo, u"MSFS2024 found and updated '%1' models")
            << models.size();
        m_loadingMessages.push_back(m);
    }

    CAircraftModelList CAircraftModelLoaderMsfs2024::performParsing()
    {
        CAircraftModelList allModels;

        // TODO TZ Implement model queries via SimConnect if possible
        // misc shut not include simconnect headers or plugins directly
        // still no idea how to do that
        const CSimulatorInfo simulatorInfo = CSimulatorInfo::msfs2024();
        allModels =
            CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getSynchronizedCachedModels(simulatorInfo);

        return allModels;
    }

    // for msfs2024, the model information is read from the SimConnect interface and no longer from the directories via
    // the aircraft.cfg
    void CAircraftModelLoaderMsfs2024::startLoadingFromDisk(
        IAircraftModelLoader::LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation,
        const QStringList &modelDirectories)
    {

        const CSimulatorInfo simulator = CSimulatorInfo::msfs2024();
        const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
        const QStringList excludedDirectoryPatterns(
            m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy

        if (mode.testFlag(LoadInBackground))
        {
            if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
            emit this->diskLoadingStarted(simulator, mode);

            // TODO TZ need help: simplify, we don't need directories in this->performParsing for MSFS2024
            m_parserWorker =
                CWorker::fromTask(this, "CAircraftModelLoaderMsfs2024::performParsing", [this, modelConsolidation]() {
                    auto models = this->performParsing();
                    if (modelConsolidation) { modelConsolidation(models, true); }
                    return models;
                });
            m_parserWorker->thenWithResult<CAircraftModelList>(this, [=](const auto &models) {
                this->updateInstalledModels(models);
                m_loadingMessages.freezeOrder();
                emit this->loadingFinished(m_loadingMessages, simulator, ParsedData);
            });
        }
        else if (mode.testFlag(LoadDirectly))
        {
            emit this->diskLoadingStarted(simulator, mode);
            CAircraftModelList models(this->performParsing());
            this->updateInstalledModels(models);
        }
    }

} // namespace swift::misc::simulation::msfs2024
