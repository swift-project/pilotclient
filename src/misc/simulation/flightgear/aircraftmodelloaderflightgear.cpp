// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/flightgear/aircraftmodelloaderflightgear.h"
#include "misc/simulation/aircraftmodel.h"
#include <QDirIterator>
namespace swift::misc::simulation::flightgear
{

    bool CAircraftModelLoaderFlightgear::isLoadingFinished() const
    {
        return !m_parserWorker || m_parserWorker->isFinished();
        ;
    }

    CAircraftModelLoaderFlightgear::CAircraftModelLoaderFlightgear(QObject *parent) : simulation::IAircraftModelLoader(simulation::CSimulatorInfo::fg(), parent)
    {}

    CAircraftModelLoaderFlightgear::~CAircraftModelLoaderFlightgear()
    {
        // that should be safe as long as the worker uses deleteLater (which it does)
        if (m_parserWorker) { m_parserWorker->waitForFinished(); }
    }

    void CAircraftModelLoaderFlightgear::updateInstalledModels(const CAircraftModelList &models)
    {
        this->setModelsForSimulator(models, CSimulatorInfo::fg());
        const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Flightgear updated '%1' models") << models.size();
        m_loadingMessages.push_back(m);
    }

    CAircraftModelList CAircraftModelLoaderFlightgear::parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
    {
        Q_UNUSED(excludeDirectories);
        if (rootDirectory.isEmpty()) { return {}; }
        CAircraftModelList installedModels;

        QDir searchPath(rootDirectory);
        searchPath.setNameFilters(QStringList() << "*-set.xml");
        QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while (aircraftIt.hasNext())
        {
            aircraftIt.next();
            if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }
            if (aircraftIt.filePath().contains("/AI/Aircraft")) { continue; }
            CAircraftModel model;
            QString modelName = aircraftIt.fileName();
            modelName = modelName.remove("-set.xml");
            model.setName(modelName);
            model.setModelString(getModelString(aircraftIt.fileName(), false));
            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
            model.setSimulator(CSimulatorInfo::fg());
            model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
            model.setModelMode(CAircraftModel::Exclude);
            addUniqueModel(model, installedModels);
        }

        return installedModels;
    }

    CAircraftModelList CAircraftModelLoaderFlightgear::parseAIAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
    {
        Q_UNUSED(excludeDirectories);
        if (rootDirectory.isEmpty()) { return {}; }

        CAircraftModelList installedModels;

        QDir searchPath(rootDirectory);
        searchPath.setNameFilters(QStringList() << "*.xml");
        QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while (aircraftIt.hasNext())
        {
            aircraftIt.next();
            if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }
            CAircraftModel model;
            QString modelName = aircraftIt.fileName();
            modelName = modelName.remove(".xml");
            model.setName(modelName);
            model.setModelString(getModelString(aircraftIt.filePath(), true));
            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
            model.setSimulator(CSimulatorInfo::fg());
            model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
            model.setModelMode(CAircraftModel::Include);
            addUniqueModel(model, installedModels);
        }

        return installedModels;
    }

    void CAircraftModelLoaderFlightgear::addUniqueModel(const CAircraftModel &model, CAircraftModelList &models)
    {
        // TODO Add check
        models.push_back(model);
    }

    CAircraftModelList CAircraftModelLoaderFlightgear::performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories)
    {
        CAircraftModelList allModels;
        for (const QString &rootDirectory : rootDirectories)
        {
            QString dir = rootDirectory;
            dir.replace('\\', '/');
            if (dir.contains("/AI/Aircraft"))
            {
                allModels.push_back(parseAIAirplanes(dir, excludeDirectories));
            }
            else
            {
                allModels.push_back(parseFlyableAirplanes(dir, excludeDirectories));
            }
        }

        return allModels;
    }

    void CAircraftModelLoaderFlightgear::startLoadingFromDisk(IAircraftModelLoader::LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
    {
        const CSimulatorInfo simulator = CSimulatorInfo::fg();
        const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
        const QStringList excludedDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy

        if (mode.testFlag(LoadInBackground))
        {
            if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
            emit this->diskLoadingStarted(simulator, mode);

            m_parserWorker = CWorker::fromTask(this, "CAircraftModelLoaderFlightgear::performParsing",
                                               [this, modelDirs, excludedDirectoryPatterns, modelConsolidation]() {
                                                   auto models = this->performParsing(modelDirs, excludedDirectoryPatterns);
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
            CAircraftModelList models(this->performParsing(modelDirs, excludedDirectoryPatterns));
            this->updateInstalledModels(models);
        }
    }

    QString CAircraftModelLoaderFlightgear::getModelString(const QString &fileName, bool ai)
    {
        QString modelString = "FG ";
        QString f;
        if (ai)
        {
            modelString.append("AI ");
            f = fileName.mid(fileName.indexOf("Aircraft") + 9);
            f.remove(".xml");
        }
        else
        {
            f = fileName;
            f.remove("-set.xml");
        }

        modelString.append(f);
        return modelString;
    }
}
