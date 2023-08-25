// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMONFUNCTIONS_H
#define BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMONFUNCTIONS_H

#include <Windows.h>
#include <QtGlobal>

namespace BlackSimPlugin::FsCommon
{
    //! @{
    //! Correctly casted values/checks
    HRESULT inline s_ok() { return S_OK; }
    HRESULT inline s_false() { return S_FALSE; }
    static bool inline isOk(HRESULT result) { return result == s_ok(); }
    static bool inline isOk(HRESULT hr1, HRESULT hr2, HRESULT hr3 = s_ok(), HRESULT hr4 = s_ok()) { return isOk(hr1) && isOk(hr2) && isOk(hr3) && isOk(hr4); }
    bool inline isFailure(HRESULT result) { return !isOk(result); }
    bool inline isFailure(HRESULT hr1, HRESULT hr2, HRESULT hr3 = s_ok(), HRESULT hr4 = s_ok()) { return !isOk(hr1, hr2, hr3, hr4); }
    bool inline dtb(double doubleBool) { return static_cast<bool>(qRound(doubleBool)); }
    //! @}
} // namespace

#endif // guard
