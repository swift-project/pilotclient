// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/input/registermetadatainput.h"

// Input headers
#include <QDBusMetaType>

#include "misc/input/actionhotkeylist.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/joystickbuttonlist.h"
#include "misc/input/keyboardkeylist.h"

namespace swift::misc::input
{
    void registerMetadata()
    {
        CActionHotkey::registerMetadata();
        CActionHotkeyList::registerMetadata();
        CHotkeyCombination::registerMetadata();
        CJoystickButton::registerMetadata();
        CJoystickButtonList::registerMetadata();
        CKeyboardKey::registerMetadata();
        CKeyboardKeyList::registerMetadata();
        qDBusRegisterMetaType<KeyCode>();
    }

} // namespace swift::misc::input
