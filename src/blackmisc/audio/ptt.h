// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
