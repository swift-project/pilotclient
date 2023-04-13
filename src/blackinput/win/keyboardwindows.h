/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYBOARD_WINDOWS_H
#define BLACKINPUT_KEYBOARD_WINDOWS_H

#include "blackinput/blackinputexport.h"
#include "blackinput/keyboard.h"
#include "blackmisc/input/keyboardkey.h"
#include "blackmisc/input/keyboardkeylist.h"
#include <QHash>
#include <QTimer>
#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include <Windows.h>

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

    protected:
        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private:
        friend class IKeyboard;

        //! Constructor
        CKeyboardWindows(QObject *parent = nullptr);

        void addKey(WPARAM vkcode);
        void removeKey(WPARAM vkcode);
        void processKeyEvent(DWORD vkCode, WPARAM event);
        void pollKeyboardState();

        const bool useWindowsHook = false;

        //! Keyboard hook procedure
        static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

        BlackMisc::Input::CHotkeyCombination m_keyCombination; //!< Set of virtual keys pressed in the last cycle
        HHOOK m_keyboardHook; //!< Keyboard hook handle
        QTimer m_pollTimer;
        QVector<int> m_pressedKeys;
    };
}

#endif // guard
