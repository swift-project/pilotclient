/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
