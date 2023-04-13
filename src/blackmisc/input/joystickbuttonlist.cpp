/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/joystickbuttonlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Input, CJoystickButton, CJoystickButtonList)

namespace BlackMisc::Input
{
    CJoystickButtonList::CJoystickButtonList(const CSequence<CJoystickButton> &baseClass) : CSequence<CJoystickButton>(baseClass)
    {}

} // namespace
