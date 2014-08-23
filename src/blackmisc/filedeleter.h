/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CFILEDELETER_H
#define BLACKMISC_CFILEDELETER_H

#include <QObject>
#include <QDebug>

namespace BlackMisc
{

    /*!
     * Utility class, deleting files when it is destroyed
     */
    class CFileDeleter
    {

    private:
        QStringList m_fileNames;

        //! File deleter
        CFileDeleter() {}

        //! add given file for deletion
        void addFile(const QString &file);

    public:

        //! file deleter
        static CFileDeleter &fileDeleter() { static CFileDeleter f; return f;}

        //! add a file (name)
        static void addFileForDeletion(const QString &file);

        //! Destructor
        ~CFileDeleter();
    };
}

#endif // guard
