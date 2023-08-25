// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/imageutils.h"

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
    return ba.toHex();
}

QPixmap BlackMisc::pngHexStringToPixmap(const QString &hexString)
{
    if (hexString.isEmpty()) { return QPixmap(); }
    QByteArray ba(QByteArray::fromHex(hexString.toLatin1()));
    return pngByteArrayToPixmap(ba);
}

bool BlackMisc::pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap)
{
    if (hexString.isEmpty()) { return false; }
    QByteArray ba(QByteArray::fromHex(hexString.toLatin1()));
    return pngByteArrayToPixmapRef(ba, pixmap);
}

QPixmap BlackMisc::iconToPixmap(const QIcon &icon)
{
    if (icon.isNull()) { return QPixmap(); }
    const QList<QSize> sizes = icon.availableSizes();
    if (!sizes.isEmpty()) { return icon.pixmap(sizes.first()); }
    return icon.pixmap(16, 16);
}
