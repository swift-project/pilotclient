// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_HOTKEYCOMBINATION_H
#define SWIFT_MISC_INPUT_HOTKEYCOMBINATION_H

#include <QMetaType>
#include <QStringList>

#include "misc/input/joystickbutton.h"
#include "misc/input/joystickbuttonlist.h"
#include "misc/input/keyboardkey.h"
#include "misc/input/keyboardkeylist.h"
#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::input, CHotkeyCombination)

namespace swift::misc::input
{
    //! Value object representing hotkey sequence
    class SWIFT_MISC_EXPORT CHotkeyCombination : public CValueObject<CHotkeyCombination>
    {
    public:
        //! Default constructor
        CHotkeyCombination() = default;

        //! Init with key
        CHotkeyCombination(CKeyboardKey key);

        //! Init with list of keys
        CHotkeyCombination(const CKeyboardKeyList &keys);

        //! Set keyboard keys
        void setKeyboardKeys(const CKeyboardKeyList &keys) { m_keyboardKeys = keys; }

        //! Get keyboard keys
        CKeyboardKeyList getKeyboardKeys() const { return m_keyboardKeys; }

        //! As keyboard key strings
        QStringList getKeyStrings() const { return m_keyboardKeys.getKeyStrings(); }

        //! Set joystick buttons
        void setJoystickButtons(const CJoystickButtonList &list) { m_joystickButtons = list; }

        //! Get joystick buttons
        CJoystickButtonList getJoystickButtons() const { return m_joystickButtons; }

        //! Add keyboard key
        void addKeyboardKey(const CKeyboardKey &key);

        //! Add joystick button
        void addJoystickButton(const CJoystickButton &button);

        //! Does combination contain key?
        bool containsKeyboardKey(const CKeyboardKey &key) const;

        //! Does combination contain button?
        bool containsJoystickButton(const CJoystickButton &button) const;

        //! Replace oldKey with newKey
        void replaceKey(CKeyboardKey oldKey, CKeyboardKey newKey);

        //! Replace oldButton with newButton
        void replaceButton(CJoystickButton oldButton, CJoystickButton newButton);

        //! Remove keyboard key
        void removeKeyboardKey(CKeyboardKey key);

        //! Remove joystick button
        void removeJoystickButton(CJoystickButton button);

        //! Is sequence empty?
        bool isEmpty() const { return m_keyboardKeys.isEmpty() && m_joystickButtons.isEmpty(); }

        //! Get size of sequence
        int size() const { return m_keyboardKeys.size() + m_joystickButtons.size(); }

        //! Is sequence a subset of other?
        //! E.g. CTRL would be a subset of CTRL+D
        bool isSubsetOf(const CHotkeyCombination &other) const;

        //! Returns the delta (removing all keys and buttons contained in other)
        CHotkeyCombination getDeltaComparedTo(const CHotkeyCombination &other) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Returns the button name with the device name prefix
        QString asStringWithDeviceNames() const;

    private:
        CKeyboardKeyList m_keyboardKeys;
        CJoystickButtonList m_joystickButtons;

        SWIFT_METACLASS(
            CHotkeyCombination,
            SWIFT_METAMEMBER(keyboardKeys),
            SWIFT_METAMEMBER(joystickButtons));
    };
} // namespace swift::misc::input

Q_DECLARE_METATYPE(swift::misc::input::CHotkeyCombination)

#endif // guard
