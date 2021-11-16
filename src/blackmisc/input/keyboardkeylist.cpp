/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/input/keycodes.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Input, CKeyboardKey, CKeyboardKeyList)

namespace BlackMisc::Input
{
    CKeyboardKeyList::CKeyboardKeyList() { }

    CKeyboardKeyList::CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass) :
        CSequence<CKeyboardKey>(baseClass)
    { }

    QStringList CKeyboardKeyList::getKeyStrings() const
    {
        QStringList keys;
        for (const CKeyboardKey &k : *this)
        {
            if (k.hasKey()) { keys.push_back(k.getKeyAsString()); }
        }
        return keys;
    }

    const CKeyboardKeyList &CKeyboardKeyList::allSupportedKeys()
    {
        static const CKeyboardKeyList allKeys =
        {
            CKeyboardKey(Key_ControlLeft),
            CKeyboardKey(Key_ControlRight),
            CKeyboardKey(Key_AltLeft),
            CKeyboardKey(Key_AltRight),
            CKeyboardKey(Key_ShiftLeft),
            CKeyboardKey(Key_ShiftRight),
            CKeyboardKey(Key_Period),
            CKeyboardKey(Key_Plus),
            CKeyboardKey(Key_Minus),
            CKeyboardKey(Key_Comma),
            CKeyboardKey(Key_Multiply),
            CKeyboardKey(Key_Divide),
            CKeyboardKey(Key_Back),
            CKeyboardKey(Key_Tab),
            CKeyboardKey(Key_Esc),
            CKeyboardKey(Key_Space),
            CKeyboardKey(Key_Delete),
            CKeyboardKey(Key_Insert),
            CKeyboardKey(Key_Home),
            CKeyboardKey(Key_End),
            CKeyboardKey(Key_PageUp),
            CKeyboardKey(Key_PageDown),
            CKeyboardKey(Key_CapsLock),
            CKeyboardKey(Key_DeadGrave),
            CKeyboardKey(Key_A),
            CKeyboardKey(Key_B),
            CKeyboardKey(Key_C),
            CKeyboardKey(Key_D),
            CKeyboardKey(Key_E),
            CKeyboardKey(Key_F),
            CKeyboardKey(Key_G),
            CKeyboardKey(Key_H),
            CKeyboardKey(Key_I),
            CKeyboardKey(Key_J),
            CKeyboardKey(Key_K),
            CKeyboardKey(Key_L),
            CKeyboardKey(Key_M),
            CKeyboardKey(Key_N),
            CKeyboardKey(Key_O),
            CKeyboardKey(Key_P),
            CKeyboardKey(Key_Q),
            CKeyboardKey(Key_R),
            CKeyboardKey(Key_S),
            CKeyboardKey(Key_T),
            CKeyboardKey(Key_U),
            CKeyboardKey(Key_V),
            CKeyboardKey(Key_W),
            CKeyboardKey(Key_X),
            CKeyboardKey(Key_Y),
            CKeyboardKey(Key_Z),
            CKeyboardKey(Key_0),
            CKeyboardKey(Key_1),
            CKeyboardKey(Key_2),
            CKeyboardKey(Key_3),
            CKeyboardKey(Key_4),
            CKeyboardKey(Key_5),
            CKeyboardKey(Key_6),
            CKeyboardKey(Key_7),
            CKeyboardKey(Key_8),
            CKeyboardKey(Key_9),
            CKeyboardKey(Key_Numpad0),
            CKeyboardKey(Key_Numpad1),
            CKeyboardKey(Key_Numpad2),
            CKeyboardKey(Key_Numpad3),
            CKeyboardKey(Key_Numpad4),
            CKeyboardKey(Key_Numpad5),
            CKeyboardKey(Key_Numpad6),
            CKeyboardKey(Key_Numpad7),
            CKeyboardKey(Key_Numpad8),
            CKeyboardKey(Key_Numpad9),
            CKeyboardKey(Key_NumpadEqual),
            CKeyboardKey(Key_NumpadMinus),
            CKeyboardKey(Key_NumpadPlus),
            CKeyboardKey(Key_NumpadDelete),
            CKeyboardKey(Key_OEM1),
            CKeyboardKey(Key_OEM2),
            CKeyboardKey(Key_OEM3),
            CKeyboardKey(Key_OEM4),
            CKeyboardKey(Key_OEM5),
            CKeyboardKey(Key_OEM6),
            CKeyboardKey(Key_OEM7),
            CKeyboardKey(Key_OEM8),
            CKeyboardKey(Key_OEM102),
            CKeyboardKey(Key_Function1),
            CKeyboardKey(Key_Function2),
            CKeyboardKey(Key_Function3),
            CKeyboardKey(Key_Function4),
            CKeyboardKey(Key_Function5),
            CKeyboardKey(Key_Function6),
            CKeyboardKey(Key_Function7),
            CKeyboardKey(Key_Function8),
            CKeyboardKey(Key_Function9),
            CKeyboardKey(Key_Function10),
            CKeyboardKey(Key_Function11),
            CKeyboardKey(Key_Function12),
            CKeyboardKey(Key_Function13),
            CKeyboardKey(Key_Function14),
            CKeyboardKey(Key_Function15),
            CKeyboardKey(Key_Function16),
            CKeyboardKey(Key_Function17),
            CKeyboardKey(Key_Function18),
            CKeyboardKey(Key_Function19),
            CKeyboardKey(Key_Function20),
            CKeyboardKey(Key_Function21),
            CKeyboardKey(Key_Function22),
            CKeyboardKey(Key_Function23),
            CKeyboardKey(Key_Function24),
        };

        return allKeys;
    }
} // ns
