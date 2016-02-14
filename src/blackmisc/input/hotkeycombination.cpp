/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include <QStringList>

namespace BlackMisc
{
    namespace Input
    {
        void CHotkeyCombination::addKeyboardKey(const CKeyboardKey &key)
        {
            if (m_keyboardKeys.contains(key)) { return; }
            m_keyboardKeys.push_back(key);
            m_keyboardKeys.sortBy(&CKeyboardKey::getKey);
        }

        void CHotkeyCombination::addJoystickButton(const CJoystickButton &button)
        {
            if (m_joystickButtons.contains(button)) { return; }
            m_joystickButtons.push_back(button);
            m_joystickButtons.sortBy(&CJoystickButton::getButtonIndex);
        }

        void CHotkeyCombination::replaceKey(const CKeyboardKey &oldKey, const CKeyboardKey &newKey)
        {
            Q_ASSERT(!oldKey.isUnknown());
            m_keyboardKeys.remove(oldKey);
            if (!newKey.isUnknown()) { m_keyboardKeys.push_back(newKey); }
            m_keyboardKeys.sortBy(&CKeyboardKey::getKey);
        }

        void CHotkeyCombination::replaceButton(const CJoystickButton &oldButton, const CJoystickButton &newButton)
        {
            m_joystickButtons.remove(oldButton);
            if (newButton.isValid()) { m_joystickButtons.push_back(newButton); }
            m_joystickButtons.sortBy(&CJoystickButton::getButtonIndex);
        }

        void CHotkeyCombination::removeKeyboardKey(const CKeyboardKey &key)
        {
            replaceKey(key, CKeyboardKey());
        }

        void CHotkeyCombination::removeJoystickButton(const CJoystickButton &button)
        {
            replaceButton(button, CJoystickButton());
        }

        bool CHotkeyCombination::isSubsetOf(const CHotkeyCombination &other) const
        {
            return std::all_of(m_keyboardKeys.begin(), m_keyboardKeys.end(), [&other](const CKeyboardKey &k) { return other.m_keyboardKeys.contains(k); }) &&
                    std::all_of(m_joystickButtons.begin(), m_joystickButtons.end(), [&other](const CJoystickButton &b) { return other.m_joystickButtons.contains(b); });
        }

        QString CHotkeyCombination::convertToQString(bool /* i18n */) const
        {
            QStringList sl;
            for (const auto &key : m_keyboardKeys)
            {
                sl << key.toQString();
            }
            for (const auto &button : m_joystickButtons)
            {
                sl << button.toQString();
            }
            return sl.join('+');
        }

    }
}
