// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/librarypath.h"
#include <QtGlobal>
#include <QString>
#include <vector>

#ifdef Q_OS_WIN
#    include <windows.h>
#endif

namespace BlackMisc
{

    void setCustomLibraryPath(const QString &path)
    {
#ifdef Q_OS_WIN
        std::wstring customLibraryPath = path.toStdWString();
        SetDllDirectory(customLibraryPath.data());
#else
        Q_UNUSED(path);
        // do nothing.
#endif
    }

    QString getCustomLibraryPath()
    {
#ifdef Q_OS_WIN
        const DWORD nBufferLength = GetDllDirectory(0, nullptr);
        std::vector<wchar_t> customLibraryPath(nBufferLength);
        GetDllDirectory(nBufferLength, customLibraryPath.data());
        return QString::fromWCharArray(customLibraryPath.data());
#else
        // do nothing.
        return {};
#endif
    }

} // ns
