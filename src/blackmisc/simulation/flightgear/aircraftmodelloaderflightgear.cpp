/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelloaderflightgear.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include <QDirIterator>
namespace BlackMisc
{
    namespace Simulation
    {
        namespace Flightgear
        {

            bool AircraftModelLoaderFlightgear::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();;
            }

            AircraftModelLoaderFlightgear::AircraftModelLoaderFlightgear(QObject *parent) : Simulation::IAircraftModelLoader(Simulation::CSimulatorInfo::fg(), parent)
            { }

            AircraftModelLoaderFlightgear::~AircraftModelLoaderFlightgear()
            {
                // that should be safe as long as the worker uses deleteLater (which it does)
                if (m_parserWorker) { m_parserWorker->waitForFinished(); }
            }

            void AircraftModelLoaderFlightgear::updateInstalledModels(const CAircraftModelList &models)
            {
                this->setModelsForSimulator(models, CSimulatorInfo::fg());
                const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Flightgear updated '%1' models") << models.size();
                m_loadingMessages.push_back(m);
            }

            Simulation::CAircraftModelList AircraftModelLoaderFlightgear::parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories);
                if (rootDirectory.isEmpty()) { return {}; }
                Simulation::CAircraftModelList installedModels;

                QDir searchPath(rootDirectory);
                searchPath.setNameFilters(QStringList() << "*-set.xml");
                QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);


                while (aircraftIt.hasNext())
                {
                    aircraftIt.next();
                    if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }
                    if (aircraftIt.filePath().toStdString().find("/AI/Aircraft") != std::string::npos) { continue; }
                    Simulation::CAircraftModel model;
                    std::string modelName = aircraftIt.fileName().toStdString();
                    modelName = modelName.substr(0, modelName.find("-set.xml"));
                    model.setName(QString::fromStdString(modelName));
                    model.setModelString(QString::fromStdString(getModelString(aircraftIt.fileName().toStdString(), false)));
                    model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                    model.setSimulator(CSimulatorInfo::fg());
                    model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
                    model.setModelMode(CAircraftModel::Exclude);

                    addUniqueModel(model, installedModels);

                }

                return installedModels;
            }

            CAircraftModelList AircraftModelLoaderFlightgear::parseAIAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories);
                if (rootDirectory.isEmpty()) { return {}; }

                Simulation::CAircraftModelList installedModels;

                QDir searchPath(rootDirectory);
                searchPath.setNameFilters(QStringList() << "*.xml");
                QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);


                while (aircraftIt.hasNext())
                {
                    aircraftIt.next();
                    if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }
                    Simulation::CAircraftModel model;
                    std::string modelName = aircraftIt.fileName().toStdString();
                    modelName = modelName.substr(0, modelName.find(".xml"));
                    model.setName(QString::fromStdString(modelName));
                    model.setModelString(QString::fromStdString(getModelString(aircraftIt.filePath().toStdString(), true)));
                    model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                    model.setSimulator(CSimulatorInfo::fg());
                    model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
                    model.setModelMode(CAircraftModel::Include);

                    addUniqueModel(model, installedModels);

                }

                return installedModels;
            }

            void AircraftModelLoaderFlightgear::addUniqueModel(const CAircraftModel &model, CAircraftModelList &models)
            {
                //TODO Add check
                models.push_back(model);
            }

            CAircraftModelList AircraftModelLoaderFlightgear::performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories)
            {
                CAircraftModelList allModels;
                for (const QString &rootDirectory : rootDirectories)
                {
                    if (QDir(rootDirectory + "/AI/Aircraft").exists())
                    {
                        allModels.push_back(parseAIAirplanes(rootDirectory + "/AI/Aircraft", excludeDirectories));
                    }
                    allModels.push_back(parseFlyableAirplanes(rootDirectory, excludeDirectories));
                }

                return allModels;
            }

            void AircraftModelLoaderFlightgear::startLoadingFromDisk(IAircraftModelLoader::LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
            {
                const CSimulatorInfo simulator = CSimulatorInfo::fg();
                const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
                const QStringList excludedDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy


                if (mode.testFlag(LoadInBackground))
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    emit this->diskLoadingStarted(simulator, mode);

                    m_parserWorker = CWorker::fromTask(this, "CAircraftModelLoaderFlightgear::performParsing",
                                                       [this, modelDirs, excludedDirectoryPatterns, modelConsolidation]()
                    {
                        auto models = this->performParsing(modelDirs, excludedDirectoryPatterns);
                        if (modelConsolidation) { modelConsolidation(models, true); }
                        return models;
                    });
                    m_parserWorker->thenWithResult<CAircraftModelList>(this, [ = ](const auto & models)
                    {
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

            std::string AircraftModelLoaderFlightgear::getModelString(std::string fileName, bool ai)
            {
                std::string modelString = "FG ";
                if (ai)
                {
                    modelString.append("AI ");
                    fileName = fileName.substr(fileName.find("Aircraft") + 9);
                    fileName = fileName.substr(0, fileName.find(".xml"));

                }
                else
                {
                    fileName = fileName.substr(0, fileName.find("-set.xml"));
                }

                modelString.append(fileName);


                return modelString;
            }


        }
    }

}

