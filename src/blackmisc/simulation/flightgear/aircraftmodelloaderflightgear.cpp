#include "aircraftmodelloaderflightgear.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include <QDirIterator>
namespace BlackMisc{
    namespace Simulation {
    namespace Flightgear {

    bool AircraftModelLoaderFlightgear::isLoadingFinished() const
    {
        return !m_parserWorker || m_parserWorker->isFinished();;
    }

    AircraftModelLoaderFlightgear::AircraftModelLoaderFlightgear(QObject *parent) : Simulation::IAircraftModelLoader (Simulation::CSimulatorInfo::fg(), parent)
        {
        std::cout << "Test";
    }

    void AircraftModelLoaderFlightgear::updateInstalledModels(const CAircraftModelList &models)
    {
        this->setModelsForSimulator(models, CSimulatorInfo::fg());
        const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"XPlane updated '%1' models") << models.size();
        m_loadingMessages.push_back(m);
    }

        Simulation::CAircraftModelList AircraftModelLoaderFlightgear::parseFlyableAirplaces(const QString &rootDirectory)
        {
            Simulation::CAircraftModelList installedModels;

            QDir searchPath(rootDirectory, fileFilterFlyable());
            QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);


            while (aircraftIt.hasNext()) {
                aircraftIt.next();
                //TODO Add possibility of exclude dir
                Simulation::CAircraftModel model;
                model.setAircraftIcaoCode(QString::fromStdString("A320"));
                model.setDescription(QString::fromStdString("Description"));
                model.setName(QString::fromStdString("ModelName"));
                model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                model.setSimulator(CSimulatorInfo::fg());
                model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
                model.setModelMode(CAircraftModel::Exclude);

                addUniqueModel(model,installedModels);

            }




            return installedModels;
        }

        const QString &AircraftModelLoaderFlightgear::fileFilterFlyable()
        {
            static const QString f("*-set.xml");
            return f;
        }

        void AircraftModelLoaderFlightgear::addUniqueModel(const CAircraftModel &model, CAircraftModelList &models)
        {
            //TODO Add check
            models.push_back(model);
        }

        void AircraftModelLoaderFlightgear::startLoadingFromDisk(IAircraftModelLoader::LoadMode mode, const IAircraftModelLoader::ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
        {
            const CSimulatorInfo simulator = CSimulatorInfo::fg();
            const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
            const QStringList excludedDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy

            if (m_parserWorker && !m_parserWorker->isFinished()){ return; }
            emit this->diskLoadingStarted(simulator, mode);

            m_parserWorker = CWorker::fromTask(this, "CAircraftModelLoaderFlightgear::performParsing",
                                                                   [this, modelDirs, excludedDirectoryPatterns, modelConsolidation]()
                                {
                                    auto models = this->parseFlyableAirplaces("X:/Flightsim/Flightgear/2018.3/data/Aircraft");
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


        }
    }

}

