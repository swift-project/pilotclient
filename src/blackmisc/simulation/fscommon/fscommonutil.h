/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMONUTIL_H
#define BLACKMISC_SIMULATION_FSCOMMONUTIL_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/blackmiscexport.h"

#include <atomic>
#include <QSet>
#include <QStringList>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! FS9/FSX/P3D utils
            class BLACKMISC_EXPORT CFsCommonUtil
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
                //! \remark only for FSX/P3D/FS9 models
                static CStatusMessageList validateAircraftConfigFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped);

                //! Validate if known SimObjects path are used
                //! \remark only for P3D
                static CStatusMessageList validateP3DSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);

                //! Validate if known SimObjects path are used
                //! \remark only for FSX
                static CStatusMessageList validateFSXSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);

            private:
                //! Validate if known SimObjects path are used
                //! \remark only for P3D/FSX
                static CStatusMessageList validateSimObjectsPath(const QSet<QString> &simObjectDirs, const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped);
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
