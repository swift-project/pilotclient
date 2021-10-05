/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/registermetadatainput.h"
#include "blackmisc/input/input.h"
#include "blackmisc/variant.h"

#include <QDBusMetaType>

namespace BlackMisc
{
    namespace Input
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
            qRegisterMetaTypeStreamOperators<KeyCode>();
        }
    }

} // ns
