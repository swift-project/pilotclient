/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSAMPLE_SAMPLEUTILS_H
#define BLACKSAMPLE_SAMPLEUTILS_H

class QString;
class QStringList;
class QTextStream;

namespace BlackMisc
{
    //! Utils for sample programms
    class CSampleUtils
    {
    public:
        //! Select directory among given ones
        static QString selectDirectory(const QStringList &directoryOptions, QTextStream &streamOut, QTextStream &streamIn);

    private:
        CSampleUtils() = delete;
    };
}

#endif // guard
