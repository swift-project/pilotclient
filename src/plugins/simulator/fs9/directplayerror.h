// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FS9_DIRECTPLAYERROR_H
#define BLACKSIMPLUGIN_FS9_DIRECTPLAYERROR_H

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#include <dplay8.h>

namespace BlackSimPlugin::Fs9
{
    //! Print the direct play error
    HRESULT logDirectPlayError(HRESULT error);
}

#endif
