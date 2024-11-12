// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMONUTIL_H
#define SWIFT_MISC_SIMULATION_FSCOMMONUTIL_H

#include "misc/simulation/aircraftmodellist.h"
#include "misc/logcategories.h"
#include "misc/swiftmiscexport.h"

#include <atomic>
#include <QSet>
#include <QStringList>

namespace swift::misc::simulation::fscommon
{
    //! FS9/FSX/P3D/MSFS utils
    class SWIFT_MISC_EXPORT CFsCommonUtil
    {
    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CFsCommonUtil() = delete;

        //! Adjust file directory
        static bool adjustFileDirectory(CAircraftModel &model, const QString &simObjectsDirectory);

        //! Adjust file directory
        static bool adjustFileDirectory(CAircraftModel &model, const QStringList &simObjectsDirectories);

        //! Copy the terrain probe
        static int copyFsxTerrainProbeFiles(const QString &simObjectDir, CStatusMessageList &messages);

        //! Validate aircraft.cfg entries (sometimes also sim.cfg)
        //! \remark only for FSX/P3D/FS9/MSFS models
        static CStatusMessageList validateAircraftConfigFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped);

        //! Validate if known SimObjects path are used
        //! \remark only for P3D
        static CStatusMessageList validateP3DSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);

        //! Validate if known SimObjects path are used
        //! \remark only for FSX
        static CStatusMessageList validateFSXSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);

        //! Validate if known SimObjects path are used
        //! \remark only for MSFS
        static CStatusMessageList validateMSFSSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);

    private:
        //! Validate if known SimObjects path are used
        //! \remark only for P3D/FSX
        static CStatusMessageList validateSimObjectsPath(const QSet<QString> &simObjectDirs, const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped);
    };
} // namespace

#endif // guard
