/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COMPRESSUTILS_H
#define BLACKMISC_COMPRESSUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QByteArray>
#include <QStringList>

class QProcess;

namespace BlackMisc
{
    //! Compress utilities
    class BLACKMISC_EXPORT CCompressUtils
    {
    public:
        CCompressUtils() = delete;

        //! Length header
        //! \remark 4 bytes -> 32bit
        static QByteArray lengthHeader(qint32 size);

        //! Unzip my using 7zip
        //! \remark relies on external 7zip command line
        static bool zip7Uncompress(const QString &file, const QString &directory, QStringList *stdOutAndError = nullptr);

        //! External program existing?
        //! \remark relies on external 7zip command line
        static bool hasZip7(QStringList *stdOutAndError = nullptr);

        //! Uses which to determine if 7Zip exists
        //! \remark for UNIX systems, using which
        static bool whichZip7(QStringList *stdOutAndError = nullptr);

    private:
        static bool runZip7Process(QProcess *zipProcess, QStringList *stdOutAndError);
    };
} // ns

#endif // guard
