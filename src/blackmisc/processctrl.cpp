/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/processctrl.h"
#include "blackmisc/buildconfig.h"
#include "blackmisc/logmessage.h"

#include <array>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace BlackMisc
{

    CProcessCtrl::CProcessCtrl(QObject *parent) :
        QProcess(parent)
    { }

    #ifdef Q_OS_WIN
    bool CProcessCtrl::startDetachedWithoutConsole(const QString &program, const QStringList &arguments)
    {
        PROCESS_INFORMATION processInfo;
        STARTUPINFOW startupInfo;
        bool inherit = true;
        DWORD flags = 0;

        QString command;
        command += program;
        command += ' ';
        command += arguments.join(' ').replace('/', '\\');

        memset (&processInfo, 0, sizeof (processInfo));
        memset (&startupInfo, 0, sizeof (startupInfo));
        startupInfo.cb = sizeof (startupInfo);

        flags |= NORMAL_PRIORITY_CLASS;
        flags |= DETACHED_PROCESS;

        Q_ASSERT(command.length() <= MAX_PATH);
        std::array<WCHAR, MAX_PATH> wszCommandLine = {{}};
        command.toWCharArray(wszCommandLine.data());
        int result = CreateProcess (nullptr, wszCommandLine.data(), 0, 0, inherit, flags, nullptr, nullptr, &startupInfo, &processInfo);

        if (result == 0)
        {
            CLogMessage(static_cast<CProcessCtrl*>(nullptr)).warning("Failed to manually launch %1: %2") << program << GetLastError();
            return false;
        }

        CloseHandle (processInfo.hProcess);
        CloseHandle (processInfo.hThread);
        return true;
    }
    #else
    bool CProcessCtrl::startDetachedWithoutConsole(const QString &program, const QStringList &arguments)
    {
        return QProcess::startDetached(program, arguments);
    }
    #endif


} // ns
