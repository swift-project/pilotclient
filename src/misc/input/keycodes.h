// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_KEYCODES_H
#define SWIFT_MISC_INPUT_KEYCODES_H

#include <QMetaType>

namespace swift::misc::input
{
    //! Key code
    //! http://www.kbdlayout.info/
    enum KeyCode
    {
        Key_Unknown,
        Key_ShiftLeft,
        Key_ShiftRight,
        Key_ControlLeft,
        Key_ControlRight,
        Key_AltLeft,
        Key_AltRight,
        Key_Back,
        Key_Tab,
        Key_Insert,
        Key_Delete,
        Key_Home,
        Key_End,
        Key_PageUp,
        Key_PageDown,
        Key_CapsLock,
        Key_Esc = 27,
        Key_Space = ' ',
        Key_DeadGrave, // message is sent for a combining key, such as a diacritic.
        Key_Plus = '+',
        Key_Comma,
        Key_Minus,
        Key_Period,
        Key_Enter,
        // 0 - 9
        Key_0 = '0',
        Key_1,
        Key_2,
        Key_3,
        Key_4,
        Key_5,
        Key_6,
        Key_7,
        Key_8,
        Key_9,
        // A - Z
        Key_A = 'A',
        Key_B,
        Key_C,
        Key_D,
        Key_E,
        Key_F,
        Key_G,
        Key_H,
        Key_I,
        Key_J,
        Key_K,
        Key_L,
        Key_M,
        Key_N,
        Key_O,
        Key_P,
        Key_Q,
        Key_R,
        Key_S,
        Key_T,
        Key_U,
        Key_V,
        Key_W,
        Key_X,
        Key_Y,
        Key_Z,
        // Numpad
        Key_Multiply,
        Key_Divide,
        Key_Numpad0,
        Key_Numpad1,
        Key_Numpad2,
        Key_Numpad3,
        Key_Numpad4,
        Key_Numpad5,
        Key_Numpad6,
        Key_Numpad7,
        Key_Numpad8,
        Key_Numpad9,
        Key_NumpadEqual,
        Key_NumpadPlus,
        Key_NumpadMinus,
        Key_NumpadDelete,
        // OEM Keys
        Key_OEM1,
        Key_OEM2,
        Key_OEM3,
        Key_OEM4,
        Key_OEM5,
        Key_OEM6,
        Key_OEM7,
        Key_OEM8,
        Key_OEM102,
        // Function keys
        Key_Function1,
        Key_Function2,
        Key_Function3,
        Key_Function4,
        Key_Function5,
        Key_Function6,
        Key_Function7,
        Key_Function8,
        Key_Function9,
        Key_Function10,
        Key_Function11,
        Key_Function12,
        Key_Function13,
        Key_Function14,
        Key_Function15,
        Key_Function16,
        Key_Function17,
        Key_Function18,
        Key_Function19,
        Key_Function20,
        Key_Function21,
        Key_Function22,
        Key_Function23,
        Key_Function24,
    };

} // namespace swift::misc::input

Q_DECLARE_METATYPE(swift::misc::input::KeyCode)

#endif // guard
