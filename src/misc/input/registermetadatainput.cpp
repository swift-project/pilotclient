// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/input/registermetadatainput.h"

// Input headers
#include "misc/input/actionhotkeylist.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/keyboardkeylist.h"
#include "misc/input/joystickbuttonlist.h"

#include <QDBusMetaType>

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

} // ns
