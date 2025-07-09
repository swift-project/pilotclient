// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COMPRESSUTILS_H
#define SWIFT_MISC_COMPRESSUTILS_H

#include <QByteArray>
#include <QStringList>

#include "misc/swiftmiscexport.h"

class QProcess;

namespace swift::misc
{
    //! Compress utilities
    class SWIFT_MISC_EXPORT CCompressUtils
    {
    public:
        CCompressUtils() = delete;

        //! Length header
        //! \remark 4 bytes -> 32bit
        static QByteArray lengthHeader(qint32 size);

        //! Unzip file
        static bool zipUncompress(const QString &file, const QString &directory, QStringList *stdOutAndError = nullptr);

    private:
        static bool runZipProcess(QProcess *zipProcess, QStringList *stdOutAndError);
    };
} // namespace swift::misc

#endif // SWIFT_MISC_COMPRESSUTILS_H
