/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/dbus.h"
#include "blackmisc/blackmiscfreefunctions.h"

const QDBusArgument &operator>>(const QDBusArgument &argument, QPixmap &pixmap)
{
    QByteArray ba;
    argument.beginStructure();
    argument >> ba;
    BlackMisc::pngByteArrayToPixmapRef(ba, pixmap);
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QPixmap &pixmap)
{
    QByteArray ba;
    BlackMisc::pixmapToPngByteArray(pixmap, ba);
    argument.beginStructure();
    argument << ba;
    argument.endStructure();
    return argument;
}
