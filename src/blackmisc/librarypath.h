/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LIBRARYPATH_H
#define BLACKMISC_LIBRARYPATH_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{

    //! Sets the custom path to additionally search when loading libraries.
    //! This does not change the default search paths.
    //! \warning This is currently Windows only. Use rpath on Unix.
    //! \sa getCustomLibraryPath
    BLACKMISC_EXPORT void setCustomLibraryPath(const QString &path);

    //! Get the custom path to search when loading libraries.
    //! Returns an empty string when no custom library path is set.
    //! \note This will not return the default search path.
    //! \warning This is currently Windows only. Use rpath on Unix.
    BLACKMISC_EXPORT QString getCustomLibraryPath();

} // ns

#endif // guard
