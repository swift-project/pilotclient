// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_OBFUSCATION_H
#define BLACKMISC_OBFUSCATION_H

#include "blackmisc/blackmiscexport.h"
#include <QString>

namespace BlackMisc
{
    //! Utility class to obfuscate strings in the source code to make them unreadable.
    //! \remark this is no real security, but as the name says just obfuscation. The sole purpose of this class is to remove potentially strings from the source code
    class BLACKMISC_EXPORT CObfuscation
    {
    public:
        //! Constructor
        CObfuscation() {}

        //! Decode string if it has the prefix, otherwise do nothing with it
        static QString decode(const QString &inString, bool trimmed = true);

        //! Encode string and return with prefix
        static QString encode(const QString &inString, bool trimmed = true);

        //! Prefix to be used with obfuscated string
        static const QString &prefix();

    private:
        static constexpr quint64 Key = 7234623562;
    };
}
#endif // guard
