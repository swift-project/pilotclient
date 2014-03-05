/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_KEYBOARD_WINDOWS_H
#define BLACKCORE_KEYBOARD_WINDOWS_H

#include "blackcore/keyboard.h"
#include "blackmisc/hwkeyboardkey.h"
#include <QHash>
#include <Windows.h>

namespace BlackCore
{
    //! \brief Windows implemenation of IKeyboard using hook procedure
    //! \todo Change QHash to a CCollection object
    class CKeyboardWindows : public IKeyboard
    {
        Q_OBJECT
    public:

        //! \brief Destructor
        virtual ~CKeyboardWindows();

        //! \copydoc IKeyboard::selectKey()
        virtual void startCapture(bool ignoreNextKey = false) override;

        //! \copydoc IKeyboard::sizeOfRegisteredFunctions()
        virtual int  sizeOfRegisteredFunctions() const override;

        //! \copydoc IKeyboard::triggerKey()
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey key, bool isPressed) override;

        //! \brief Keyboard hook handle
        HHOOK keyboardHook() const { return m_keyboardHook; }

        //! \private
        void keyEvent(WPARAM vkCode, uint event);

    protected:

        friend class IKeyboard;

        //! \brief Constructor
        CKeyboardWindows(QObject *parent = nullptr);

        //! \brief Copy Constructor
        CKeyboardWindows(CKeyboardWindows const&);

        //! \copydoc IKeyboard::init()
        virtual bool init() override;

        //! \brief Assignment operator
        void operator=(CKeyboardWindows const&);

        //! \copydoc IKeyboard::registerHotKeyImpl()
        virtual IKeyboard::RegistrationHandle registerHotkeyImpl(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, std::function<void(bool)> function) override;

        //! \copydoc IKeyboard::unregisterHotkeyImpl()
        virtual void unregisterHotkeyImpl(const IKeyboard::RegistrationHandle &handle) override;

        //! \copydoc IKeyboard::unregisterHotkeyImpl()
        virtual void unregisterAllHotkeysImpl() override;

    private:

        /*!
         * \brief Constructor
         * \param keySet
         * \param isFinished
         */
        void sendCaptureNotification(const BlackMisc::Hardware::CKeyboardKey &key, bool isFinished);

        /*!
         * \brief Calls registered functions on keyboard event
         * \param keySet
         * \param isPressed
         */
        void callFunctionsBy(const BlackMisc::Hardware::CKeyboardKey &keySet, bool isPressed);

        void addKey(WPARAM vkcode);
        void removeKey(WPARAM vkcode);

        /*!
         * \brief Keyboard hook procedure
         * \param nCode
         * \param wParam
         * \param lParam
         * \return
         */
        static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);


        QHash<BlackMisc::Hardware::CKeyboardKey, QList<IKeyboard::RegistrationHandle>> m_registeredFunctions; //!< Registered hotkey functions
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        HHOOK m_keyboardHook;                   //!< Keyboard hook handle
        Mode m_mode;                            //!< Operation mode
    };
}

#endif // BLACKCORE_KEYBOARD_WINDOWS_H
