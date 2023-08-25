// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesp3d.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackmisc/stringutils.h"
#include <QTextStream>

using namespace BlackMisc::Simulation::FsCommon;

namespace BlackSample
{
    void CSamplesP3D::samplesMisc(QTextStream &streamOut)
    {
        QSet<QString> configFiles = CFsDirectories::findP3dAddOnConfigFiles("v5");
        if (configFiles.isEmpty()) { configFiles = CFsDirectories::findP3dAddOnConfigFiles("v4"); }

        streamOut << BlackMisc::joinStringSet(configFiles, ", ") << Qt::endl;

        const QSet<QString> addOnPaths = CFsDirectories::allConfigFilesPathValues(configFiles.values(), false, {});
        streamOut << BlackMisc::joinStringSet(addOnPaths, ", ") << Qt::endl;

        const QSet<QString> simObjectPaths = CFsDirectories::fsxSimObjectsPaths("B:/fsx.cfg", false);
        streamOut << BlackMisc::joinStringSet(simObjectPaths, ", ") << Qt::endl;
    }
} // namespace
