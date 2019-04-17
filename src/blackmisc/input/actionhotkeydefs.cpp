/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "actionhotkeydefs.h"

namespace BlackMisc
{
    namespace Input
    {
        const QString &pttHotkeyAction()
        {
            static const QString s("/Voice/Activate push-to-talk");
            return s;
        }

        const QPixmap &pttHotkeyIcon()
        {
            return CIcons::radio16();
        }
    } // ns
} // ns
