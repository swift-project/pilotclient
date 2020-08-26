/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ATOMICFILE_H
#define BLACKMISC_ATOMICFILE_H

#include "blackmisc/blackmiscexport.h"

#include <QFile>
#include <QFileDevice>
#include <QIODevice>
#include <QString>
#include <exception>

namespace BlackMisc
{
    /*!
     * A subclass of QFile which writes to a temporary file while it is open, then renames the file
     * when it is closed, so that it overwrites the target file as a single, atomic transaction.
     *
     * If the application crashes while data is still being written, the original file is unchanged.
     *
     * \fixme Consider using QSaveFile.
     */
    class BLACKMISC_EXPORT CAtomicFile : public QFile
    {
        Q_OBJECT

    public:
        //! \copydoc QFile::QFile(const QString &)
        CAtomicFile(const QString &filename) : QFile(filename) {}

        //! \copydoc QFile::~QFile
        virtual ~CAtomicFile() override;

        //! \copydoc QFile::open
        //! Just before opening the file, the filename is changed so we actually write to a temporary file.
        virtual bool open(OpenMode mode) override;

        //! \copydoc QFileDevice::close
        //! After closing the file, it is renamed so that it overwrites the target file.
        virtual void close() override;

        //! Calls close() and returns false if there was an error at any stage.
        bool checkedClose();

        //! Closes the file without renaming it.
        void abandon();

        //! \copydoc QFileDevice::error
        FileError error() const;

        //! \copydoc QFileDevice::unsetError
        void unsetError();

    private:
        static QString randomSuffix();
        void replaceOriginal();

        QString m_originalFilename;
        bool m_renameError = false;
        bool m_permissionError = false;
    };
}

#endif
