/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodelloader.h"
#include <QPointer>

namespace BlackMisc::Simulation::Flightgear
{
    //! Flightgear aircraft model loader
    class CAircraftModelLoaderFlightgear : public Simulation::IAircraftModelLoader
    {
        Q_OBJECT

    public:
        //! Constructor
        CAircraftModelLoaderFlightgear(QObject *parent = nullptr);

        //! Virtual destructor
        virtual ~CAircraftModelLoaderFlightgear() override;

        //! Parsed or injected models
        void updateInstalledModels(const CAircraftModelList &models);

        //! \copydoc IAircraftModelLoader::isLoadingFinished
        virtual bool isLoadingFinished() const override;

    protected:
        //! \copydoc IAircraftModelLoader::startLoadingFromDisk
        virtual void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories) override;

    private:
        QString getModelString(const QString &filePath, bool ai);
        Simulation::CAircraftModelList parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
        Simulation::CAircraftModelList parseAIAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
        void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models);
        QPointer<CWorker> m_parserWorker;
        CAircraftModelList performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories);
    };
}
