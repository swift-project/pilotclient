/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#include "samplesfile.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;

namespace BlackSample
{
    int CSamplesFile::samples(QTextStream &out)
    {
        const QString dir = "R:/temp";
        out << "Testing directory search " << dir << Qt::endl;
        const bool f = CDirectoryUtils::containsFileInDir(dir, "*.air", true);
        out << "Found " << boolToYesNo(f) << Qt::endl;
        return 0;
    }

} // namespace
