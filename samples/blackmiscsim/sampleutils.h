// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSAMPLE_SAMPLEUTILS_H
#define BLACKSAMPLE_SAMPLEUTILS_H

#include <QStringList>

class QString;
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
