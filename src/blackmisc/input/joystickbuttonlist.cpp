// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/input/joystickbuttonlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Input, CJoystickButton, CJoystickButtonList)

namespace BlackMisc::Input
{
    CJoystickButtonList::CJoystickButtonList(const CSequence<CJoystickButton> &baseClass) : CSequence<CJoystickButton>(baseClass)
    {}

} // namespace
