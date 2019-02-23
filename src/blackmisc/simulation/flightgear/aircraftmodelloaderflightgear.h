#include "blackmisc/simulation/aircraftmodelloader.h"
#include <QPointer>

namespace BlackMisc{
    namespace Simulation {
        namespace Flightgear {
        class AircraftModelLoaderFlightgear : public Simulation::IAircraftModelLoader{
            Q_OBJECT

            // IAircraftModelLoader interface
        public:


            //Constructor
            AircraftModelLoaderFlightgear(QObject *parent = nullptr);

            virtual ~AircraftModelLoaderFlightgear() override;

            // Interface functions
            // Parsed or injected models
            void updateInstalledModels(const CAircraftModelList &models);
            bool isLoadingFinished() const;

            protected:
            // Interface functions
            void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories) override;

        private:
            std::string getModelString(std::string filePath,bool ai);
            Simulation::CAircraftModelList parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
            Simulation::CAircraftModelList parseAIAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
            static const QString &fileFilterFlyable();
            void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models);
            QPointer<CWorker> m_parserWorker;
            CAircraftModelList performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories);


        };
        }
    }


}