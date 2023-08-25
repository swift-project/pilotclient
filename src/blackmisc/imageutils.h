// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_IMAGEUTILS_H
#define BLACKMISC_IMAGEUTILS_H

#include "blackmisc/blackmiscexport.h"

#include <QByteArray>
#include <QString>
#include <QPixmap>
#include <QIcon>

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! Pixmap to PNG byte array
    BLACKMISC_EXPORT bool pixmapToPngByteArray(const QPixmap &pixmap, QByteArray &array);

    //! Pixmap from PNG byte array
    BLACKMISC_EXPORT QPixmap pngByteArrayToPixmap(const QByteArray &array);

    //! Pixmap from PNG byte array
    BLACKMISC_EXPORT bool pngByteArrayToPixmapRef(const QByteArray &array, QPixmap &pixmap);

    //! Pixmap as HEX string (for PNG image)
    BLACKMISC_EXPORT QString pixmapToPngHexString(const QPixmap &pixmap);

    //! Hex encoded pixmap string to Pixmap
    BLACKMISC_EXPORT QPixmap pngHexStringToPixmap(const QString &hexString);

    //! Hex encoded pixmap string to Pixmap
    BLACKMISC_EXPORT bool pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap);

    //! Icon to pixmap
    BLACKMISC_EXPORT QPixmap iconToPixmap(const QIcon &icon);
} // ns

#endif // guard
