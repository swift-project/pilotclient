/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_PTT_H
#define BLACKMISC_AUDIO_PTT_H

#include <QObject>

namespace BlackMisc::Audio
{
    //! Voice COM channel
    enum PTTCOM
    {
        COM1,
        COM2,
        COMActive,
        COMUnspecified
    };
}

Q_DECLARE_METATYPE(BlackMisc::Audio::PTTCOM)

#endif // guard
