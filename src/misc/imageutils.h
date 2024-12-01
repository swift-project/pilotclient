// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_IMAGEUTILS_H
#define SWIFT_MISC_IMAGEUTILS_H

#include <QByteArray>
#include <QIcon>
#include <QPixmap>
#include <QString>

#include "misc/swiftmiscexport.h"

//! Free functions in swift::misc
namespace swift::misc
{
    //! Pixmap to PNG byte array
    SWIFT_MISC_EXPORT bool pixmapToPngByteArray(const QPixmap &pixmap, QByteArray &array);

    //! Pixmap from PNG byte array
    SWIFT_MISC_EXPORT QPixmap pngByteArrayToPixmap(const QByteArray &array);

    //! Pixmap from PNG byte array
    SWIFT_MISC_EXPORT bool pngByteArrayToPixmapRef(const QByteArray &array, QPixmap &pixmap);

    //! Pixmap as HEX string (for PNG image)
    SWIFT_MISC_EXPORT QString pixmapToPngHexString(const QPixmap &pixmap);

    //! Hex encoded pixmap string to Pixmap
    SWIFT_MISC_EXPORT QPixmap pngHexStringToPixmap(const QString &hexString);

    //! Hex encoded pixmap string to Pixmap
    SWIFT_MISC_EXPORT bool pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap);

    //! Icon to pixmap
    SWIFT_MISC_EXPORT QPixmap iconToPixmap(const QIcon &icon);
} // namespace swift::misc

#endif // SWIFT_MISC_IMAGEUTILS_H
