/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FS9_DIRECTPLAYERROR_H
#define BLACKSIMPLUGIN_FS9_DIRECTPLAYERROR_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <dplay8.h>

namespace BlackSimPlugin::Fs9
{
    //! Print the direct play error
    HRESULT logDirectPlayError(HRESULT error);
}

#endif
