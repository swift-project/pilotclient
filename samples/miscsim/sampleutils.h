// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SAMPLE_SAMPLEUTILS_H
#define SWIFT_SAMPLE_SAMPLEUTILS_H

#include <QStringList>

class QString;
class QTextStream;

namespace swift::misc
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
} // namespace swift::misc

#endif // guard
