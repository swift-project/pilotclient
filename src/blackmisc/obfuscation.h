/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
