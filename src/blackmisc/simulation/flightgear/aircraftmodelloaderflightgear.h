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
            Simulation::CAircraftModelList parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
            static const QString &fileFilterFlyable();
            void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models);
            QPointer<CWorker> m_parserWorker;
            CAircraftModelList performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories);

        protected:
            void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories);
        };
        }
    }


}

