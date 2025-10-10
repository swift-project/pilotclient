// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/imageutils.h"

#include <QBuffer>
#include <QIODevice>

bool swift::misc::pixmapToPngByteArray(const QPixmap &pixmap, QByteArray &array)
{
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    bool s = pixmap.save(&buffer, "PNG");
    buffer.close();
    return s;
}

QPixmap swift::misc::pngByteArrayToPixmap(const QByteArray &array)
{
    if (array.isEmpty()) { return {}; }
    QPixmap p;
    bool s = p.loadFromData(array, "PNG");
    return s ? p : QPixmap();
}

bool swift::misc::pngByteArrayToPixmapRef(const QByteArray &array, QPixmap &pixmap)
{
    if (array.isEmpty()) { return false; }
    bool s = pixmap.loadFromData(array, "PNG");
    return s;
}

QString swift::misc::pixmapToPngHexString(const QPixmap &pixmap)
{
    QByteArray ba;
    bool s = pixmapToPngByteArray(pixmap, ba);
    if (!s) { return {}; }
    return ba.toHex();
}

QPixmap swift::misc::pngHexStringToPixmap(const QString &hexString)
{
    if (hexString.isEmpty()) { return {}; }
    QByteArray ba(QByteArray::fromHex(hexString.toLatin1()));
    return pngByteArrayToPixmap(ba);
}

bool swift::misc::pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap)
{
    if (hexString.isEmpty()) { return false; }
    QByteArray ba(QByteArray::fromHex(hexString.toLatin1()));
    return pngByteArrayToPixmapRef(ba, pixmap);
}

QPixmap swift::misc::iconToPixmap(const QIcon &icon)
{
    if (icon.isNull()) { return {}; }
    const QList<QSize> sizes = icon.availableSizes();
    if (!sizes.isEmpty()) { return icon.pixmap(sizes.first()); }
    return icon.pixmap(16, 16);
}
