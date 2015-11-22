/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STRINGUTILS_H
#define BLACKMISC_STRINGUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QMap>
#include <QByteArray>
#include <QString>

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! A map converted to string
    template<class K, class V> QString qmapToString(const QMap<K, V> &map)
    {
        QString s;
        const QString kv("%1: %2 ");
        QMapIterator<K, V> i(map);
        while (i.hasNext())
        {
            i.next();
            s.append(
                kv.arg(i.key()).arg(i.value())
            );
        }
        return s.trimmed();
    }

    //! Bool to on/off
    BLACKMISC_EXPORT QString boolToOnOff(bool v, bool  i18n = false);

    //! Bool to yes / no
    BLACKMISC_EXPORT QString boolToYesNo(bool v, bool  i18n = false);

    //! Bool to true / false
    BLACKMISC_EXPORT QString boolToTrueFalse(bool v, bool  i18n = false);

    //! Convert string to bool
    BLACKMISC_EXPORT bool stringToBool(const QString &boolString);

    //! Int to hex value
    BLACKMISC_EXPORT QString intToHex(int value, int digits = 2);

    //! Int to hex value (per byte, 2 digits)
    BLACKMISC_EXPORT QString bytesToHexString(const QByteArray &bytes);

    //! Byte array from hex value string per byte, 2 digits
    BLACKMISC_EXPORT QByteArray byteArrayFromHexString(const QString &hexString);

} // ns

#endif // guard
