// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKMISC_SIMULATION_FLIGHTGEAR_AIRCRAFTMODELLOADERFLIGHTGEAR_H
#define BLACKMISC_SIMULATION_FLIGHTGEAR_AIRCRAFTMODELLOADERFLIGHTGEAR_H

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

#endif // BLACKMISC_SIMULATION_FLIGHTGEAR_AIRCRAFTMODELLOADERFLIGHTGEAR_H
