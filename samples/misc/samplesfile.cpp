// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "samplesfile.h"

#include "misc/directoryutils.h"
#include "misc/stringutils.h"

using namespace swift::misc;

namespace swift::sample
{
    int CSamplesFile::samples(QTextStream &out)
    {
        const QString dir = "R:/temp";
        out << "Testing directory search " << dir << Qt::endl;
        const bool f = CDirectoryUtils::containsFileInDir(dir, "*.air", true);
        out << "Found " << boolToYesNo(f) << Qt::endl;
        return 0;
    }

} // namespace swift::sample
