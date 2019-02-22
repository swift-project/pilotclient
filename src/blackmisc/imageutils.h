/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
