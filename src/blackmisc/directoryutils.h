/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIRECTORYUTILS_H
#define BLACKMISC_DIRECTORYUTILS_H

#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    /*!
     * Utility class for directory operations
     */
    class BLACKMISC_EXPORT CDirectoryUtils
    {
    public:
        //! Returns the directory of the application. In contrast to QCoreApplication::applicationDirPath()
        //! it takes Mac OS X app bundles into account and returns the directory of the bundle.
        static QString applicationDirectoryPath();

        //! Returns the application directory of the calling executable as normalized string.
        //! \note There is no trailing '/'.
        //! \warning The normalization rules are implementation specific and could change over time.
        static const QString &normalizedApplicationDirectory();
    };

} // ns

#endif // guard
