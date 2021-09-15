/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/ping.h"
#include "blackconfig/buildconfig.h"
#include <QProcess>

using namespace BlackConfig;

namespace BlackMisc::Network
{
    bool canPing(const QString &hostAddress)
    {
        if (hostAddress.isEmpty()) { return false; }
        QProcess process;
        process.setProgram("ping");
        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            process.setArguments({ "-n", "1", hostAddress });
        }
        else
        {
            // all UNIX alike
            process.setArguments({ "-c", "1", hostAddress });
        }
        process.start();
        process.waitForFinished();
        const int rc = process.exitCode();
        if (rc != 0) { return false; }

        const QString std = process.readAllStandardOutput();
        const QString err = process.readAllStandardError();
        if (std.contains("unreachable", Qt::CaseInsensitive)) { return false; }
        if (err.contains("unreachable", Qt::CaseInsensitive)) { return false; }
        return true;
    }

    bool canPing(const QUrl &url)
    {
        if (url.isEmpty()) { return false; }
        return canPing(url.host());
    }
}
