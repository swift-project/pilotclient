/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include <QFile>
#include "filedeleter.h"

namespace BlackMisc
{
    void CFileDeleter::addFile(const QString &file)
    {
        if (file.isEmpty()) return;
        if (!this->m_fileNames.contains(file)) this->m_fileNames.append(file);
    }

    void CFileDeleter::addFileForDeletion(const QString &file)
    {
        CFileDeleter::fileDeleter().addFile(file);
    }

    CFileDeleter::~CFileDeleter()
    {
        foreach(const QString fn, this->m_fileNames)
        {
            QFile f(fn);
            f.remove();
        }
    }
}
