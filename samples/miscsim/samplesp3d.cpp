// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesp3d.h"
#include "misc/simulation/fscommon/fsdirectories.h"
#include "misc/stringutils.h"
#include <QTextStream>

using namespace swift::misc::simulation::fscommon;

namespace swift::sample
{
    void CSamplesP3D::samplesMisc(QTextStream &streamOut)
    {
        QSet<QString> configFiles = CFsDirectories::findP3dAddOnConfigFiles("v5");
        if (configFiles.isEmpty()) { configFiles = CFsDirectories::findP3dAddOnConfigFiles("v4"); }

        streamOut << swift::misc::joinStringSet(configFiles, ", ") << Qt::endl;

        const QSet<QString> addOnPaths = CFsDirectories::allConfigFilesPathValues(configFiles.values(), false, {});
        streamOut << swift::misc::joinStringSet(addOnPaths, ", ") << Qt::endl;

        const QSet<QString> simObjectPaths = CFsDirectories::fsxSimObjectsPaths("B:/fsx.cfg", false);
        streamOut << swift::misc::joinStringSet(simObjectPaths, ", ") << Qt::endl;
    }
} // namespace
