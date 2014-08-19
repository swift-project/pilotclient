/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystick_linux.h"

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    CJoystickLinux::CJoystickLinux(QObject *parent) :
        IJoystick(parent)
    {
    }

    CJoystickLinux::~CJoystickLinux()
    {
    }

    void CJoystickLinux::startCapture()
    {
    }

    void CJoystickLinux::triggerButton(const CJoystickButton button, bool isPressed)
    {
        if(!isPressed) emit buttonUp(button);
        else emit buttonDown(button);
    }

} // namespace BlackInput
