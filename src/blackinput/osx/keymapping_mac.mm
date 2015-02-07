/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keymapping_mac.h"
#include <QDebug>
#include <Carbon/Carbon.h>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    Qt::Key CKeyMappingMac::convertToKey(unsigned int virtualKey)
    {
        switch (virtualKey)
        {
        case kVK_ANSI_0: return Qt::Key_0;
        case kVK_ANSI_1: return Qt::Key_1;
        case kVK_ANSI_2: return Qt::Key_2;
        case kVK_ANSI_3: return Qt::Key_3;
        case kVK_ANSI_4: return Qt::Key_4;
        case kVK_ANSI_5: return Qt::Key_5;
        case kVK_ANSI_6: return Qt::Key_6;
        case kVK_ANSI_7: return Qt::Key_7;
        case kVK_ANSI_8: return Qt::Key_8;
        case kVK_ANSI_9: return Qt::Key_9;
        case kVK_ANSI_A: return Qt::Key_A;
        case kVK_ANSI_B: return Qt::Key_B;
        case kVK_ANSI_C: return Qt::Key_C;
        case kVK_ANSI_D: return Qt::Key_D;
        case kVK_ANSI_E: return Qt::Key_E;
        case kVK_ANSI_F: return Qt::Key_F;
        case kVK_ANSI_G: return Qt::Key_G;
        case kVK_ANSI_H: return Qt::Key_H;
        case kVK_ANSI_I: return Qt::Key_I;
        case kVK_ANSI_J: return Qt::Key_J;
        case kVK_ANSI_K: return Qt::Key_K;
        case kVK_ANSI_L: return Qt::Key_L;
        case kVK_ANSI_M: return Qt::Key_M;
        case kVK_ANSI_N: return Qt::Key_N;
        case kVK_ANSI_O: return Qt::Key_O;
        case kVK_ANSI_P: return Qt::Key_P;
        case kVK_ANSI_Q: return Qt::Key_Q;
        case kVK_ANSI_R: return Qt::Key_R;
        case kVK_ANSI_S: return Qt::Key_S;
        case kVK_ANSI_T: return Qt::Key_T;
        case kVK_ANSI_U: return Qt::Key_U;
        case kVK_ANSI_V: return Qt::Key_V;
        case kVK_ANSI_W: return Qt::Key_W;
        case kVK_ANSI_X: return Qt::Key_X;
        case kVK_ANSI_Y: return Qt::Key_Y;
        case kVK_ANSI_Z: return Qt::Key_Z;
        default: return Qt::Key_unknown;
        }
    }

} // namespace BlackInput
