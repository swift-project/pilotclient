/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_HOTKEYCOMBINATION_H
#define BLACKMISC_INPUT_HOTKEYCOMBINATION_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/input/joystickbuttonlist.h"
#include "blackmisc/input/keyboardkey.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QStringList>

namespace BlackMisc
{
    namespace Input
    {
        //! Value object representing hotkey sequence
        class BLACKMISC_EXPORT CHotkeyCombination : public CValueObject<CHotkeyCombination>
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
            void addKeyboardKey(CKeyboardKey key);

            //! Add joystick button
            void addJoystickButton(CJoystickButton button);

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

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            CKeyboardKeyList m_keyboardKeys;
            CJoystickButtonList m_joystickButtons;

            BLACK_METACLASS(
                CHotkeyCombination,
                BLACK_METAMEMBER(keyboardKeys),
                BLACK_METAMEMBER(joystickButtons)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Input::CHotkeyCombination)

#endif // guard
