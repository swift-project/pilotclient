/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_LINUX_H
#define BLACKINPUT_KEYBOARD_LINUX_H

#include "blackinput/keyboard.h"
#include "blackmisc/hwkeyboardkey.h"
#include <QHash>

namespace BlackInput
{
    //! \brief Linux implemenation of IKeyboard using hook procedure
    //! \todo Change QHash to a CCollection object
    class CKeyboardLinux : public IKeyboard
    {
        Q_OBJECT
    public:

        //! \brief Destructor
        virtual ~CKeyboardLinux();

        //! \copydoc IKeyboard::selectKey()
        virtual void startCapture(bool ignoreNextKey = false) override;

        //! \copydoc IKeyboard::sizeOfRegisteredFunctions()
        virtual int  sizeOfRegisteredFunctions() const override;

        //! \copydoc IKeyboard::triggerKey()
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey key, bool isPressed) override;

    protected:

        friend class IKeyboard;

        //! \brief Constructor
        CKeyboardLinux(QObject *parent = nullptr);

        //! \brief Copy Constructor
        CKeyboardLinux(CKeyboardLinux const&);

        //! \copydoc IKeyboard::init()
        virtual bool init() override;

        //! \brief Assignment operator
        void operator=(CKeyboardLinux const&);

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


        QHash<BlackMisc::Hardware::CKeyboardKey, QList<IKeyboard::RegistrationHandle>> m_registeredFunctions; //!< Registered hotkey functions
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        Mode m_mode;                            //!< Operation mode
    };
}

#endif // BLACKINPUT_KEYBOARD_LINUX_H
