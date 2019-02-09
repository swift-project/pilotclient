#include "blackmisc/simulation/aircraftmodelloader.h"
#include <QPointer>

namespace BlackMisc{
    namespace Simulation {
        namespace Flightgear {
        class AircraftModelLoaderFlightgear : public Simulation::IAircraftModelLoader{


            // IAircraftModelLoader interface
        public:
            bool isLoadingFinished() const;
            AircraftModelLoaderFlightgear(QObject *parent = nullptr);

            //! Parsed or injected models
            void updateInstalledModels(const CAircraftModelList &models);

        private:
            Simulation::CAircraftModelList parseFlyableAirplaces(const QString &rootDirectory);
            static const QString &fileFilterFlyable();
            void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models);
            QPointer<CWorker> m_parserWorker;

        protected:
            void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories);
        };
        }
    }


}

