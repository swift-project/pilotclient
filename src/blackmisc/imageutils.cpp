/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/imageutils.h"
#include "blackmisc/stringutils.h"

#include <QBuffer>
#include <QIODevice>

bool BlackMisc::pixmapToPngByteArray(const QPixmap &pixmap, QByteArray &array)
{
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    bool s = pixmap.save(&buffer, "PNG");
    buffer.close();
    return s;
}

QPixmap BlackMisc::pngByteArrayToPixmap(const QByteArray &array)
{
    if (array.isEmpty()) { return QPixmap(); }
    QPixmap p;
    bool s = p.loadFromData(array, "PNG");
    return s ? p : QPixmap();
}

bool BlackMisc::pngByteArrayToPixmapRef(const QByteArray &array, QPixmap &pixmap)
{
    if (array.isEmpty()) { return false; }
    bool s = pixmap.loadFromData(array, "PNG");
    return s;
}

QString BlackMisc::pixmapToPngHexString(const QPixmap &pixmap)
{
    QByteArray ba;
    bool s = pixmapToPngByteArray(pixmap, ba);
    if (!s) { return QString(); }
    return bytesToHexString(ba);
}

QPixmap BlackMisc::pngHexStringToPixmap(const QString &hexString)
{
    if (hexString.isEmpty()) { return QPixmap(); }
    QByteArray ba(byteArrayFromHexString(hexString));
    return pngByteArrayToPixmap(ba);
}

bool BlackMisc::pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap)
{
    if (hexString.isEmpty()) { return false; }
    QByteArray ba(byteArrayFromHexString(hexString));
    return pngByteArrayToPixmapRef(ba, pixmap);
}

QPixmap BlackMisc::iconToPixmap(const QIcon &icon)
{
    if (icon.isNull()) { return QPixmap(); }
    const QList<QSize> sizes = icon.availableSizes();
    if (!sizes.isEmpty()) { return icon.pixmap(sizes.first()); }
    return icon.pixmap(16, 16);
}
