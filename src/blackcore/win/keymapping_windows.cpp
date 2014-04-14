/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keymapping_windows.h"
#include <QDebug>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

using namespace BlackMisc::Hardware;

namespace BlackCore
{
    Qt::Key CKeyMappingWindows::convertToKey(WPARAM virtualKey)
    {
        switch (virtualKey)
        {
        case '0': return Qt::Key_0; break;
        case '1': return Qt::Key_1; break;
        case '2': return Qt::Key_2; break;
        case '3': return Qt::Key_3; break;
        case '4': return Qt::Key_4; break;
        case '5': return Qt::Key_5; break;
        case '6': return Qt::Key_6; break;
        case '7': return Qt::Key_7; break;
        case '8': return Qt::Key_8; break;
        case '9': return Qt::Key_9; break;
        case 'A': return Qt::Key_A; break;
        case 'B': return Qt::Key_B; break;
        case 'C': return Qt::Key_C; break;
        case 'D': return Qt::Key_D; break;
        case 'E': return Qt::Key_E; break;
        case 'F': return Qt::Key_F; break;
        case 'G': return Qt::Key_G; break;
        case 'H': return Qt::Key_H; break;
        case 'I': return Qt::Key_I; break;
        case 'J': return Qt::Key_J; break;
        case 'K': return Qt::Key_K; break;
        case 'L': return Qt::Key_L; break;
        case 'M': return Qt::Key_M; break;
        case 'N': return Qt::Key_N; break;
        case 'O': return Qt::Key_O; break;
        case 'P': return Qt::Key_P; break;
        case 'Q': return Qt::Key_Q; break;
        case 'R': return Qt::Key_R; break;
        case 'S': return Qt::Key_S; break;
        case 'T': return Qt::Key_T; break;
        case 'U': return Qt::Key_U; break;
        case 'V': return Qt::Key_V; break;
        case 'W': return Qt::Key_W; break;
        case 'X': return Qt::Key_X; break;
        case 'Y': return Qt::Key_Y; break;
        case 'Z': return Qt::Key_Z; break;
        default:  return Qt::Key_unknown; break;
        }
    }

    CKeyboardKey::Modifier CKeyMappingWindows::convertToModifier(WPARAM virtualKey)
    {
        switch (virtualKey)
        {
        qDebug() << virtualKey;
        case VK_LSHIFT: return CKeyboardKey::ModifierShiftLeft; break;
        case VK_RSHIFT: return CKeyboardKey::ModifierShiftRight; break;
        case VK_LCONTROL: return CKeyboardKey::ModifierCtrlLeft; break;
        case VK_RCONTROL: return CKeyboardKey::ModifierCtrlRight; break;
        case VK_LMENU: return CKeyboardKey::ModifierAltLeft; break;
        case VK_RMENU: return CKeyboardKey::ModifierAltRight; break;
        default: return CKeyboardKey::ModifierNone; break;
        }
    }

    bool CKeyMappingWindows::isModifier(WPARAM vkcode)
    {
        switch (vkcode)
        {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
            return true;
        default: return false;
        }
    }

} // namespace BlackCore
