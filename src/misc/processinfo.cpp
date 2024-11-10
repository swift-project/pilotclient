// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/processinfo.h"
#include <QFile>
#include <QFileInfo>
#include <type_traits>

#if defined(Q_OS_MACOS)
#    include <libproc.h>
#elif defined(Q_OS_WIN)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#    include <psapi.h>
#endif

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CProcessInfo)

namespace swift::misc
{

    QString CProcessInfo::convertToQString(bool) const
    {
        return QStringLiteral("{ %1, %2 }").arg(QString::number(m_pid), m_name);
    }

#if defined(Q_OS_LINUX)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        QString path = QFileInfo(QStringLiteral("/proc/%1/exe").arg(pid)).symLinkTarget();
        return QFileInfo(path).fileName();
    }
#elif defined(Q_OS_MACOS)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        char name[1024];
        proc_name(pid, name, std::extent_v<decltype(name)>);
        return name;
    }
#elif defined(Q_OS_WIN)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
        if (!proc) { return {}; }
        wchar_t path[1024];
        auto len = GetModuleFileNameEx(proc, nullptr, path, std::extent_v<decltype(path)>);
        CloseHandle(proc);
        if (len <= 0) { return {}; }
        return QFileInfo(QString::fromWCharArray(path)).completeBaseName();
    }
#else
    QString CProcessInfo::processNameFromId(qint64)
    {
        qFatal("Not implemented");
        return {};
    }
#endif

}
