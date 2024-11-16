// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_ACTIONHOTKEYDEFS_H
#define SWIFT_MISC_INPUT_ACTIONHOTKEYDEFS_H

#include <QString>

#include "misc/icons.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::input
{
    //! PTT key
    SWIFT_MISC_EXPORT const QString &pttHotkeyAction();

    //! PTT key
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex pttHotkeyIcon();

    //! Audio volume + key
    SWIFT_MISC_EXPORT const QString &audioVolumeIncreaseHotkeyAction();

    //! Audio volume - key
    SWIFT_MISC_EXPORT const QString &audioVolumeDecreaseHotkeyAction();

    //! Audio volume + key
    SWIFT_MISC_EXPORT const QString &audioVolumeIncreaseCom1HotkeyAction();

    //! Audio volume - key
    SWIFT_MISC_EXPORT const QString &audioVolumeDecreaseCom1HotkeyAction();

    //! Audio volume + key
    SWIFT_MISC_EXPORT const QString &audioVolumeIncreaseCom2HotkeyAction();

    //! Audio volume - key
    SWIFT_MISC_EXPORT const QString &audioVolumeDecreaseCom2HotkeyAction();

    //! Audio icon volume +
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex audioVolumeIncreaseHotkeyIcon();

    //! Audio icon volume -
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex audioVolumeDecreaseHotkeyIcon();

    //! XPDR state
    SWIFT_MISC_EXPORT const QString &toggleXPDRStateHotkeyAction();

    //! XPDR state
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex toggleXPDRStateHotkeyIcon();

    //! XPDR state
    SWIFT_MISC_EXPORT const QString &toggleXPDRIdentHotkeyAction();

    //! XPDR ident
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex toggleXPDRIdentHotkeyIcon();

    //! Radar zoom in
    SWIFT_MISC_EXPORT const QString &radarZoomInHotkeyAction();

    //! Radar zoom in
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex radarZoomInHotkeyIcon();

    //! Radar zoom out
    SWIFT_MISC_EXPORT const QString &radarZoomOutHotkeyAction();

    //! Radar zoom out
    SWIFT_MISC_EXPORT swift::misc::CIcons::IconIndex radarZoomOutHotkeyIcon();

} // namespace swift::misc::input

#endif
