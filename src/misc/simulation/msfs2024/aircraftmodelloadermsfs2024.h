// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_SIMULATION_MSFS2024_AIRCRAFTMODELLOADERMSFS2024_H
#define SWIFT_MISC_SIMULATION_MSFS2024_AIRCRAFTMODELLOADERMSFS2024_H

#include <QPointer>

#include "misc/simulation/aircraftmodelloader.h"

namespace swift::misc::simulation::msfs2024
{
    //! Msfs2024 aircraft model loader
    class CAircraftModelLoaderMsfs2024 : public IAircraftModelLoader
    {
        Q_OBJECT

    public:
        //! Constructor
        CAircraftModelLoaderMsfs2024(QObject *parent = nullptr);

        //! Virtual destructor
        ~CAircraftModelLoaderMsfs2024() override;

        //! Parsed or injected models
        void updateInstalledModels(const CAircraftModelList &models);

        //! \copydoc IAircraftModelLoader::isLoadingFinished
        bool isLoadingFinished() const override;

    protected:
        //! \copydoc IAircraftModelLoader::startLoadingFromDisk
        void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation,
                                  const QStringList &modelDirectories) override;

    private:
        QPointer<CWorker> m_parserWorker;
        // CAircraftModelList performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories);
        CAircraftModelList performParsing();
    };

} // namespace swift::misc::simulation::msfs2024

#endif // SWIFT_MISC_SIMULATION_MSFS2024_AIRCRAFTMODELLOADERMSFS2024_H
