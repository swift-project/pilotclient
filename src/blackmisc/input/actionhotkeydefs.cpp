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

        const QString &pttCom1HotkeyAction()
        {
            static const QString s("/Voice/Activate push-to-talk COM1");
            return s;
        }

        const QString &pttCom2HotkeyAction()
        {
            static const QString s("/Voice/Activate push-to-talk COM2");
            return s;
        }

        const QPixmap &audioVolumeDecreaseHotkeyIcon()
        {
            return CIcons::volumeLow16();
        }

        const QPixmap &audioVolumeIncreaseHotkeyIcon()
        {
            return CIcons::volumeHigh16();
        }

        const QString &audioVolumeDecreaseHotkeyAction()
        {
            static const QString s("/Audio/Volume decrease");
            return s;
        }

        const QString &audioVolumeIncreaseHotkeyAction()
        {
            static const QString s("/Audio/Volume increase");
            return s;
        }
    } // ns
} // ns
