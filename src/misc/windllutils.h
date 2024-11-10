// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WINDLLUTILS_H
#define SWIFT_MISC_WINDLLUTILS_H

#include "misc/swiftmiscexport.h"
#include <QString>
#include <QList>

namespace swift::misc
{
    //! Functions dealing with Windows DLLs
    class SWIFT_MISC_EXPORT CWinDllUtils
    {
    public:
        //! No ctor
        CWinDllUtils() = delete;

        //! Info about an DLL
        struct DLLInfo
        {
            QString iso; //!< language iso code
            QString errorMsg; //!< error message if any
            QString fileVersion; //!< file version, like 3.2.1.3
            QString fileDescription; //!< file description
            QString productVersion; //!< product version, like 3.2.1.3
            QString productVersionName; //!< version and name
            QString productName; //!< product name
            QString originalFilename; //!< original filename
            QString fullFilename; //!< full filename and path

            //! Product or file version
            const QString &productOrFileVersion() const
            {
                if (!productVersionName.isEmpty()) { return productVersionName; }
                if (!productVersion.isEmpty()) { return productVersion; }
                return fileVersion;
            }

            //! Summary
            QString summary() const
            {
                if (!errorMsg.isEmpty()) { return errorMsg; }
                static const QString s("Product: '%1' version: %2 language: %3 filename: '%4'");
                return s.arg(productName, productOrFileVersion(), iso, fullFilename);
            }
        };

        //! Process modules
        struct ProcessModule
        {
            QString moduleName; //!< module / dll name
            QString executable; //!< full executable path
            qint64 processId = -1; //!< pid
        };

        //! Get DDL
        static DLLInfo getDllInfo(const QString &dllFile);

        //! Process modules per id
        static QList<ProcessModule> getModules(qint64 processId = -1, const QString &nameFilter = {});
    };
} // ns
#endif // guard
