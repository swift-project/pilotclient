/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keymapping_linux.h"
#include <QDebug>
#include <linux/input.h>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    Qt::Key CKeyMappingLinux::convertToKey(int virtualKey)
    {
        switch (virtualKey)
        {
            case KEY_0: return Qt::Key_0; break;
            case KEY_1: return Qt::Key_1; break;
            case KEY_2: return Qt::Key_2; break;
            case KEY_3: return Qt::Key_3; break;
            case KEY_4: return Qt::Key_4; break;
            case KEY_5: return Qt::Key_5; break;
            case KEY_6: return Qt::Key_6; break;
            case KEY_7: return Qt::Key_7; break;
            case KEY_8: return Qt::Key_8; break;
            case KEY_9: return Qt::Key_9; break;
            case KEY_A: return Qt::Key_A; break;
            case KEY_B: return Qt::Key_B; break;
            case KEY_C: return Qt::Key_C; break;
            case KEY_D: return Qt::Key_D; break;
            case KEY_E: return Qt::Key_E; break;
            case KEY_F: return Qt::Key_F; break;
            case KEY_G: return Qt::Key_G; break;
            case KEY_H: return Qt::Key_H; break;
            case KEY_I: return Qt::Key_I; break;
            case KEY_J: return Qt::Key_J; break;
            case KEY_K: return Qt::Key_K; break;
            case KEY_L: return Qt::Key_L; break;
            case KEY_M: return Qt::Key_M; break;
            case KEY_N: return Qt::Key_N; break;
            case KEY_O: return Qt::Key_O; break;
            case KEY_P: return Qt::Key_P; break;
            case KEY_Q: return Qt::Key_Q; break;
            case KEY_R: return Qt::Key_R; break;
            case KEY_S: return Qt::Key_S; break;
            case KEY_T: return Qt::Key_T; break;
            case KEY_U: return Qt::Key_U; break;
            case KEY_V: return Qt::Key_V; break;
            case KEY_W: return Qt::Key_W; break;
            case KEY_X: return Qt::Key_X; break;
            case KEY_Y: return Qt::Key_Y; break;
            case KEY_Z: return Qt::Key_Z; break;
            default:  return Qt::Key_unknown; break;
        }
    }

    CKeyboardKey::Modifier CKeyMappingLinux::convertToModifier(int virtualKey)
    {
        switch (virtualKey)
        {
            case KEY_LEFTSHIFT: return CKeyboardKey::ModifierShiftLeft; break;
            case KEY_RIGHTSHIFT: return CKeyboardKey::ModifierShiftRight; break;
            case KEY_LEFTCTRL: return CKeyboardKey::ModifierCtrlLeft; break;
            case KEY_RIGHTCTRL: return CKeyboardKey::ModifierCtrlRight; break;
            case KEY_LEFTALT: return CKeyboardKey::ModifierAltLeft; break;
            case KEY_RIGHTALT: return CKeyboardKey::ModifierAltRight; break;
            default: return CKeyboardKey::ModifierNone; break;
        }
    }

    bool CKeyMappingLinux::isModifier(int virtualKey)
    {
        switch (virtualKey)
        {
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            return true;
        default: return false;
        }
    }

    bool CKeyMappingLinux::isMouseButton(int virtualKey)
    {
        switch (virtualKey)
        {
            case BTN_LEFT:
            case BTN_RIGHT:
            case BTN_MIDDLE:
                return true;
            default:
                return false;
        }
    }

} // namespace BlackInput
