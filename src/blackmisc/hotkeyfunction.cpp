/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "hotkeyfunction.h"
#include "QCoreApplication"

namespace BlackMisc
{
    CHotkeyFunction::CHotkeyFunction()
        : m_function(HotkeyNone)
    {
    }

    CHotkeyFunction::CHotkeyFunction(Function function)
        : m_function(function)
    {
    }

    QString CHotkeyFunction::getFunctionAsString() const
    {
        switch (m_function)
        {
        case HotkeyNone: return "";
        case HotkeyPtt: return qApp->translate("CHotkeyFunction", "PTT");
        case HotkeyOpacity50: return qApp->translate("CHotkeyFunction", "Opacity 50%");
        case HotkeyOpacity100: return qApp->translate("CHotkeyFunction", "Opacity 100%");
        case HotkeyToggleCom1: return qApp->translate("CHotkeyFunction", "Toggle COM1");
        case HotkeyToggleCom2: return qApp->translate("CHotkeyFunction", "Toggle COM2");
        case HotkeyToogleWindowsStayOnTop: return qApp->translate("CHotkeyFunction", "Toogle Window on top");
        default:
            qFatal("Wrong function");
            return "";
        }
    }

    QString CHotkeyFunction::convertToQString(bool /* i18n */) const
    {
        return getFunctionAsString();
    }
}
