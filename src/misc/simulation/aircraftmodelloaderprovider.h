// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_IAIRCRAFTMODELLOADERPROVIDER_H
#define SWIFT_MISC_SIMULATION_IAIRCRAFTMODELLOADERPROVIDER_H

#include "misc/simulation/aircraftmodelloader.h"

namespace swift::misc::simulation
{
    /*!
     * Single instances of all model loaders (lazy init)
     */
    class SWIFT_MISC_EXPORT CMultiAircraftModelLoaderProvider : public QObject
    {
        Q_OBJECT

    public:
        //! Create a loader and synchronize caches
        static IAircraftModelLoader *createModelLoader(const CSimulatorInfo &simulator, QObject *parent = nullptr);

        //! Loader instance
        IAircraftModelLoader *loaderInstance(const CSimulatorInfo &simulator);

        //! Singleton
        static CMultiAircraftModelLoaderProvider &multiModelLoaderInstance();

        //! @{
        //! Simulator specific loaders
        IAircraftModelLoader *modelLoaderFsx() const { return m_loaderFsx; }
        IAircraftModelLoader *modelLoaderP3D() const { return m_loaderP3D; }
        IAircraftModelLoader *modelLoaderXP() const { return m_loaderXP; }
        IAircraftModelLoader *modelLoaderFS9() const { return m_loaderFS9; }
        IAircraftModelLoader *modelLoaderFG() const { return m_loaderFG; }
        IAircraftModelLoader *modelLoaderMsfs() const { return m_loaderMsfs; }
        //! @}

    signals:
        //! \copydoc IAircraftModelLoader::loadingFinished
        void loadingFinished(const swift::misc::CStatusMessageList &status, const CSimulatorInfo &simulator,
                             IAircraftModelLoader::LoadFinishedInfo info);

        //! \copydoc IAircraftModelLoader::diskLoadingStarted
        void diskLoadingStarted(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode mode);

        //! \copydoc IAircraftModelLoader::loadingProgress
        void loadingProgress(const CSimulatorInfo &simulator, const QString &message, int progressPercentage);

        //! \copydoc IAircraftModelLoader::cacheChanged
        void cacheChanged(const CSimulatorInfo &simulator);

    private:
        IAircraftModelLoader *m_loaderFsx = nullptr;
        IAircraftModelLoader *m_loaderP3D = nullptr;
        IAircraftModelLoader *m_loaderXP = nullptr;
        IAircraftModelLoader *m_loaderFS9 = nullptr;
        IAircraftModelLoader *m_loaderFG = nullptr;
        IAircraftModelLoader *m_loaderMsfs = nullptr;
        IAircraftModelLoader *m_loaderMsfs2024 = nullptr;

        //! Init the loader
        IAircraftModelLoader *initLoader(const CSimulatorInfo &simulator);
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_IAIRCRAFTMODELLOADERPROVIDER_H
