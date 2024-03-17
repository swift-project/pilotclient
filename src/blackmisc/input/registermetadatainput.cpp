// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/input/registermetadatainput.h"

// Input headers
#include "blackmisc/input/actionhotkeylist.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/input/joystickbuttonlist.h"

#include <QDBusMetaType>

namespace BlackMisc::Input
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
