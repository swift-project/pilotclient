/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/atomicfile.h"
#include "blackmisc/algorithm.h"

#include <QDir>
#include <QFileInfo>
#include <QFlags>
#include <random>
#include <type_traits>

#if defined(Q_OS_POSIX)
#include <stdio.h>
#elif defined(Q_OS_WIN32)
#include <windows.h>
#endif

namespace BlackMisc
{

    bool CAtomicFile::open(CAtomicFile::OpenMode mode)
    {
        m_originalFilename = fileName();
        QFileInfo fileInfo(fileName());
        setFileName(QFileInfo(fileInfo.dir(), ".tmp." + fileInfo.fileName() + "." + randomSuffix()).filePath());
        if (exists()) { remove(); }

        bool ok = true;
        if (mode & ReadOnly)
        {
            if (exists(m_originalFilename) && ! copy(m_originalFilename, fileName())) { ok = false; }
        }

        if (ok && ! QFile::open(mode)) { ok = false; }
        if (! ok) { setFileName(m_originalFilename); }
        return ok;
    }

    void CAtomicFile::close()
    {
        if (! isOpen()) { return; }

        QFile::close();

        if (error() == NoError) { replaceOriginal(); }
        setFileName(m_originalFilename);
    }

    bool CAtomicFile::checkedClose()
    {
        close();
        return error() == NoError;
    }

    void CAtomicFile::abandon()
    {
        if (! isOpen()) { return; }

        QFile::close();
        remove();
        setFileName(m_originalFilename);
    }

    CAtomicFile::FileError CAtomicFile::error() const
    {
        if (m_renameError) { return RenameError; }
        return QFile::error();
    }

    void CAtomicFile::unsetError()
    {
        m_renameError = false;
        QFile::unsetError();
    }

    QString CAtomicFile::randomSuffix()
    {
        constexpr auto max = 2176782335;
        return QString::number(std::uniform_int_distribution<std::decay_t<decltype(max)>>(0, max)(Private::defaultRandomGenerator()), 36);
    }

#if defined(Q_OS_POSIX)
    void CAtomicFile::replaceOriginal()
    {
        auto result = ::rename(qPrintable(fileName()), qPrintable(m_originalFilename));
        if (result < 0) { m_renameError = true; }
    }
#elif defined(Q_OS_WIN32)
    void CAtomicFile::replaceOriginal()
    {
        auto result = MoveFileExA(qPrintable(fileName()), qPrintable(m_originalFilename), MOVEFILE_REPLACE_EXISTING);
        if (! result) { m_renameError = true; }
    }
#else
    void CAtomicFile::replaceOriginal()
    {
        if (exists(m_originalFilename)) { remove(m_originalFilename); }
        rename(m_originalFilename);
    }
#endif

}
