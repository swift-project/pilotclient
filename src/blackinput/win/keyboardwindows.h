/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_WINDOWS_H
#define BLACKINPUT_KEYBOARD_WINDOWS_H

#include "blackinput/blackinputexport.h"
#include "blackinput/keyboard.h"
#include "blackmisc/input/keyboardkey.h"
#include "blackmisc/input/keyboardkeylist.h"
#include <QHash>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace BlackInput
{
    //! Windows implemenation of IKeyboard using hook procedure
    class BLACKINPUT_EXPORT CKeyboardWindows : public IKeyboard
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CKeyboardWindows(CKeyboardWindows const &) = delete;

        //! Assignment operator
        CKeyboardWindows &operator=(CKeyboardWindows const &) = delete;

        //! Destructor
        virtual ~CKeyboardWindows() override;

        //! Keyboard hook handle
        HHOOK keyboardHook() const { return m_keyboardHook; }

        //! \private
        void processKeyEvent(WPARAM vkCode, uint event);

    protected:
        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private:
        friend class IKeyboard;

        //! Constructor
        CKeyboardWindows(QObject *parent = nullptr);

        void addKey(WPARAM vkcode);
        void removeKey(WPARAM vkcode);

        //! Keyboard hook procedure
        static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

        BlackMisc::Input::CHotkeyCombination m_keyCombination; //!< Set of virtual keys pressed in the last cycle
        HHOOK m_keyboardHook; //!< Keyboard hook handle
    };
}

#endif // guard
