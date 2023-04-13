/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
