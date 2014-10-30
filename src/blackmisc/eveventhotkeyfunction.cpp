/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "eveventhotkeyfunction.h"

namespace BlackMisc
{
    namespace Event
    {
        CEventHotkeyFunction::CEventHotkeyFunction(CHotkeyFunction func, bool argument)
            : m_hotkeyFunc(func), m_hotkeyFuncArgument(argument)
        {
        }

        /*
         * Convert to string
         */
        QString CEventHotkeyFunction::convertToQString(bool i18n) const
        {
            QString s;
            s.append(m_eventOriginator.toQString(i18n));
            s.append(" ").append(m_hotkeyFunc.toQString(i18n));
            return s;
        }
    }
}
