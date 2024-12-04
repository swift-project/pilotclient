// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_KEYBOARD_WINDOWS_H
#define SWIFT_INPUT_KEYBOARD_WINDOWS_H

#include <QHash>
#include <QTimer>

#include "input/keyboard.h"
#include "input/swiftinputexport.h"
#include "misc/input/keyboardkey.h"
#include "misc/input/keyboardkeylist.h"
#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include <Windows.h>

namespace swift::input
{
    //! Windows implemenation of IKeyboard using hook procedure
    class SWIFT_INPUT_EXPORT CKeyboardWindows : public IKeyboard
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

        swift::misc::input::CHotkeyCombination m_keyCombination; //!< Set of virtual keys pressed in the last cycle
        HHOOK m_keyboardHook; //!< Keyboard hook handle
        QTimer m_pollTimer;
        QVector<int> m_pressedKeys;
    };
} // namespace swift::input

#endif // SWIFT_INPUT_KEYBOARD_WINDOWS_H
