// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/obfuscation.h"

#include <QStringBuilder>

#include "simplecrypt/simplecrypt.h"

namespace swift::misc
{
    QString CObfuscation::decode(const QString &inString, bool trimmed)
    {
        if (!inString.startsWith(prefix())) { return trimmed ? inString.trimmed() : inString; }
        if (inString.length() == prefix().length()) { return QString(); }
        SimpleCrypt simpleCrypt(Key);
        const QString decoded = simpleCrypt.decryptToString(inString.mid(prefix().length()));
        return trimmed ? decoded.trimmed() : decoded;
    }

    QString CObfuscation::encode(const QString &inString, bool trimmed)
    {
        SimpleCrypt simpleCrypt(Key);
        const QString encrypted = simpleCrypt.encryptToString(trimmed ? inString.trimmed() : inString);
        return prefix() % encrypted;
    }

    const QString &CObfuscation::prefix()
    {
        static const QString obfuscated("OBF:");
        return obfuscated;
    }
} // namespace swift::misc
