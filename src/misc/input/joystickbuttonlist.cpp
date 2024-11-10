// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/input/joystickbuttonlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::input, CJoystickButton, CJoystickButtonList)

namespace swift::misc::input
{
    CJoystickButtonList::CJoystickButtonList(const CSequence<CJoystickButton> &baseClass) : CSequence<CJoystickButton>(baseClass)
    {}

} // namespace
