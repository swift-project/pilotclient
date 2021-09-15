/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H
#define BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H

#include "blackmisc/icons.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc::Input
{
    //! PTT key
    BLACKMISC_EXPORT const QString &pttHotkeyAction();

    //! PTT key
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex pttHotkeyIcon();

    //! Audio volume + key
    BLACKMISC_EXPORT const QString &audioVolumeIncreaseHotkeyAction();

    //! Audio volume - key
    BLACKMISC_EXPORT const QString &audioVolumeDecreaseHotkeyAction();

    //! Audio volume + key
    BLACKMISC_EXPORT const QString &audioVolumeIncreaseCom1HotkeyAction();

    //! Audio volume - key
    BLACKMISC_EXPORT const QString &audioVolumeDecreaseCom1HotkeyAction();

    //! Audio volume + key
    BLACKMISC_EXPORT const QString &audioVolumeIncreaseCom2HotkeyAction();

    //! Audio volume - key
    BLACKMISC_EXPORT const QString &audioVolumeDecreaseCom2HotkeyAction();

    //! Audio icon volume +
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex audioVolumeIncreaseHotkeyIcon();

    //! Audio icon volume -
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex audioVolumeDecreaseHotkeyIcon();

    //! XPDR state
    BLACKMISC_EXPORT const QString &toggleXPDRStateHotkeyAction();

    //! XPDR state
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex toggleXPDRStateHotkeyIcon();

    //! XPDR state
    BLACKMISC_EXPORT const QString &toggleXPDRIdentHotkeyAction();

    //! XPDR ident
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex toggleXPDRIdentHotkeyIcon();

    //! Radar zoom in
    BLACKMISC_EXPORT const QString &radarZoomInHotkeyAction();

    //! Radar zoom in
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex radarZoomInHotkeyIcon();

    //! Radar zoom out
    BLACKMISC_EXPORT const QString &radarZoomOutHotkeyAction();

    //! Radar zoom out
    BLACKMISC_EXPORT BlackMisc::CIcons::IconIndex radarZoomOutHotkeyIcon();

} // ns

#endif // guard
