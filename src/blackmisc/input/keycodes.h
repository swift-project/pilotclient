/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_KEYCODES_H
#define BLACKMISC_INPUT_KEYCODES_H

#include <QMetaType>

namespace BlackMisc
{
    namespace Input
    {
        //! Key code
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
            Key_Esc,
            Key_Plus = 43,
            Key_Comma,
            Key_Minus,
            Key_Period,
            // 0 - 9
            Key_0 = 48,
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
            Key_A = 65,
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
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Input::KeyCode)

#endif //guard
