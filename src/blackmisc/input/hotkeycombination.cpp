/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/iterator.h"

#include <QStringList>
#include <QtGlobal>
#include <algorithm>
#include <tuple>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Input, CHotkeyCombination)

namespace BlackMisc::Input
{
    CHotkeyCombination::CHotkeyCombination(CKeyboardKey key) : CHotkeyCombination(CKeyboardKeyList { key })
    { }

    CHotkeyCombination::CHotkeyCombination(const CKeyboardKeyList &keys) : m_keyboardKeys(keys)
    { }

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

    bool CHotkeyCombination::containsKeyboardKey(const CKeyboardKey &key) const
    {
        return m_keyboardKeys.contains(key);
    }

    bool CHotkeyCombination::containsJoystickButton(const CJoystickButton &button) const
    {
        return m_joystickButtons.contains(button);
    }

    void CHotkeyCombination::replaceKey(CKeyboardKey oldKey, CKeyboardKey newKey)
    {
        if (oldKey.hasKey())
        {
            Q_ASSERT(!oldKey.isUnknown());
            m_keyboardKeys.remove(oldKey);
        }
        if (!newKey.isUnknown()) { m_keyboardKeys.push_back(newKey); }
        m_keyboardKeys.sortBy(&CKeyboardKey::getKey);
    }

    void CHotkeyCombination::replaceButton(CJoystickButton oldButton, CJoystickButton newButton)
    {
        m_joystickButtons.remove(oldButton);
        if (newButton.isValid()) { m_joystickButtons.push_back(newButton); }
        m_joystickButtons.sortBy(&CJoystickButton::getButtonIndex);
    }

    void CHotkeyCombination::removeKeyboardKey(CKeyboardKey key)
    {
        replaceKey(key, CKeyboardKey());
    }

    void CHotkeyCombination::removeJoystickButton(CJoystickButton button)
    {
        replaceButton(button, CJoystickButton());
    }

    bool CHotkeyCombination::isSubsetOf(const CHotkeyCombination &other) const
    {
        return std::all_of(m_keyboardKeys.begin(), m_keyboardKeys.end(), [&other](const CKeyboardKey & k) { return other.m_keyboardKeys.contains(k); }) &&
                std::all_of(m_joystickButtons.begin(), m_joystickButtons.end(), [&other](const CJoystickButton & b) { return other.m_joystickButtons.contains(b); });
    }

    CHotkeyCombination CHotkeyCombination::getDeltaComparedTo(const CHotkeyCombination &other) const
    {
        CHotkeyCombination combination(*this);
        const CKeyboardKeyList otherKeys = other.getKeyboardKeys();
        for (const CKeyboardKey &key : otherKeys)
        {
            if (containsKeyboardKey(key)) { combination.removeKeyboardKey(key); }
        }

        const CJoystickButtonList otherButtons = other.getJoystickButtons();
        for (const CJoystickButton &button : otherButtons)
        {
            if (containsJoystickButton(button)) { combination.removeJoystickButton(button); }
        }
        return combination;
    }

    QString CHotkeyCombination::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        QStringList sl;
        sl.reserve(m_keyboardKeys.size() + m_joystickButtons.size());
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

    QString CHotkeyCombination::asStringWithDeviceNames() const
    {
        QStringList sl;
        sl.reserve(m_keyboardKeys.size() + m_joystickButtons.size());
        for (const auto &key : m_keyboardKeys)
        {
            sl << key.toQString();
        }
        for (const auto &button : m_joystickButtons)
        {
            sl << button.getButtonAsStringWithDeviceName();
        }
        return sl.join('+');
    }
} // ns
