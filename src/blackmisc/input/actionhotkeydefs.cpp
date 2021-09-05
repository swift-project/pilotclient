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

        CIcons::IconIndex pttHotkeyIcon()
        {
            return CIcons::StandardIconRadio16;
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

        CIcons::IconIndex audioVolumeDecreaseHotkeyIcon()
        {
            return CIcons::StandardIconVolumeLow16;
        }

        CIcons::IconIndex audioVolumeIncreaseHotkeyIcon()
        {
            return CIcons::StandardIconVolumeHigh16;
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

        const QString &audioVolumeDecreaseCom1HotkeyAction()
        {
            static const QString s("/Audio/Volume (COM1) decrease");
            return s;
        }

        const QString &audioVolumeIncreaseCom1HotkeyAction()
        {
            static const QString s("/Audio/Volume (COM1) increase");
            return s;
        }

        const QString &audioVolumeDecreaseCom2HotkeyAction()
        {
            static const QString s("/Audio/Volume (COM2) decrease");
            return s;
        }

        const QString &audioVolumeIncreaseCom2HotkeyAction()
        {
            static const QString s("/Audio/Volume (COM2) increase");
            return s;
        }


        CIcons::IconIndex toggleXPDRStateHotkeyIcon()
        {
            return CIcons::StandardIconRadio16;
        }

        CIcons::IconIndex toggleXPDRIdentHotkeyIcon()
        {
            return CIcons::StandardIconRadio16;
        }

        const QString &toggleXPDRStateHotkeyAction()
        {
            static const QString s("/Own aircraft/Toggle XPDR state");
            return s;
        }

        const QString &toggleXPDRIdentHotkeyAction()
        {
            static const QString s("/Own aircraft/XPDR ident");
            return s;
        }

        const QString &radarZoomInHotkeyAction()
        {
            static const QString s("/Radar/Zoom in");
            return s;
        }

        CIcons::IconIndex radarZoomInHotkeyIcon()
        {
            return CIcons::StandardIconZoomIn16;
        }

        const QString &radarZoomOutHotkeyAction()
        {
            static const QString s("/Radar/Zoom out");
            return s;
        }

        CIcons::IconIndex radarZoomOutHotkeyIcon()
        {
            return CIcons::StandardIconZoomOut16;
        }

    } // ns
} // ns
