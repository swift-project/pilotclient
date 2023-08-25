// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
