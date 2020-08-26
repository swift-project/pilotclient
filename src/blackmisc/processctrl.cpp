/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/processctrl.h"
#include "blackmisc/logmessage.h"
#include <QStringBuilder>
#include <array>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace BlackMisc
{
    CProcessCtrl::CProcessCtrl(QObject *parent) :
        QProcess(parent)
    { }

    #ifdef Q_OS_WIN
    bool startDetachedWithConsoleWindow(const QString &program, const QStringList &arguments)
    {
        bool inherit = false;

        PROCESS_INFORMATION processInfo;
        memset (&processInfo, 0, sizeof (processInfo));

        STARTUPINFO startupInfo;
        memset (&startupInfo, 0, sizeof (startupInfo));
        startupInfo.cb = sizeof (startupInfo);

        QString command = '"' % QString(program).replace('/', '\\') % '"';
        if (!arguments.isEmpty())
        {
            command += " \"" % arguments.join("\" \"").replace('/', '\\') % '"';
        }

        DWORD flags = 0;
        flags |= NORMAL_PRIORITY_CLASS;
        flags |= CREATE_UNICODE_ENVIRONMENT;
        flags |= CREATE_NEW_CONSOLE;

        Q_ASSERT(command.length() <= MAX_PATH);
        std::array<WCHAR, MAX_PATH> wszCommandLine = {{}};
        command.toWCharArray(wszCommandLine.data());

        int result = CreateProcess (nullptr, wszCommandLine.data(), nullptr, nullptr, inherit, flags, nullptr, nullptr, &startupInfo, &processInfo);

        if (result == 0)
        {
            CLogMessage(static_cast<CProcessCtrl*>(nullptr)).warning(u"Failed to start %1: %2") << program << GetLastError();
            return false;
        }

        CloseHandle (processInfo.hProcess);
        CloseHandle (processInfo.hThread);
        return true;
    }
    #else
    bool startDetachedWithConsoleWindow(const QString &program, const QStringList &arguments)
    {
        //! \fixme Handle Linux and OS X here
        return QProcess::startDetached(program, arguments);
    }
    #endif

    bool CProcessCtrl::startDetached(const QString &program, const QStringList &arguments, bool withConsoleWindow)
    {
        if (withConsoleWindow) { return startDetachedWithConsoleWindow(program, arguments); }
        else { return QProcess::startDetached(program, arguments); }
    }

} // ns
