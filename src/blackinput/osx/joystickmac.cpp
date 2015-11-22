/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystickmac.h"

using namespace BlackMisc::Input;

namespace BlackInput
{
    CJoystickMac::CJoystickMac(QObject *parent) :
        IJoystick(parent)
    {
    }

    CJoystickMac::~CJoystickMac()
    {
    }

} // namespace BlackInput
