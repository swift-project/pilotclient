// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/ping.h"
#include "config/buildconfig.h"
#include <QProcess>

using namespace swift::config;

namespace swift::misc::network
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
