/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMONFUNCTIONS_H
#define BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMONFUNCTIONS_H

#include <Windows.h>
#include <QtGlobal>

namespace BlackSimPlugin::FsCommon
{
    //! @{
    //! Correctly casted values/checks
    HRESULT inline  s_ok() { return S_OK; }
    HRESULT inline  s_false() { return S_FALSE; }
    static bool inline isOk(HRESULT result) { return result == s_ok(); }
    static bool inline isOk(HRESULT hr1, HRESULT hr2, HRESULT hr3 = s_ok(), HRESULT hr4 = s_ok()) { return isOk(hr1) && isOk(hr2) && isOk(hr3) && isOk(hr4); }
    bool inline isFailure(HRESULT result) { return !isOk(result); }
    bool inline isFailure(HRESULT hr1, HRESULT hr2, HRESULT hr3 = s_ok(), HRESULT hr4 = s_ok()) { return !isOk(hr1, hr2, hr3, hr4); }
    bool inline dtb(double doubleBool) { return static_cast<bool>(qRound(doubleBool)); }
    //! @}
} // namespace

#endif // guard
