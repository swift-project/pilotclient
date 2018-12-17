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

#include "blackmisc/blackmiscexport.h"

#include <QObject>
#include <QString>
#include <QStringList>

class QTimerEvent;

namespace BlackMisc
{
    /*!
     * Utility class, deleting files when it is destroyed
     */
    class BLACKMISC_EXPORT CFileDeleter
    {
    public:
        //! File deleter
        CFileDeleter() {}

        //! add a file (name)
        void addFileForDeletion(const QString &file);

        //! Add files (names)
        void addFilesForDeletion(const QStringList &files);

        //! Destructor
        ~CFileDeleter();

        //! Delete files
        void deleteFiles();

        //! Not copyable.
        //! @{
        CFileDeleter(const CFileDeleter &) = delete;
        CFileDeleter &operator =(const CFileDeleter &) = delete;
        //! @}

    private:
        QStringList m_fileNames;
    };

    /*!
     * Utility class, deleting files after time
     */
    class BLACKMISC_EXPORT CTimedFileDeleter : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CTimedFileDeleter(const QString &file, int deleteAfterMs, QObject *parent = nullptr);

    protected:
        //! \copydoc QObject::timerEvent
        virtual void timerEvent(QTimerEvent *event) override;

    private:
        CFileDeleter m_fileDeleter;
        int m_timerId = -1;
    };

} // ns

#endif // guard
