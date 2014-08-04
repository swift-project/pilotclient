/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_WINDOWS_H
#define BLACKINPUT_KEYBOARD_WINDOWS_H

#include "blackinput/keyboard.h"
#include "blackmisc/hwkeyboardkey.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include <QHash>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace BlackInput
{
    //! \brief Windows implemenation of IKeyboard using hook procedure
    //! \todo Change QHash to a CCollection object
    class CKeyboardWindows : public IKeyboard
    {
        Q_OBJECT
    public:

        //! \brief Destructor
        virtual ~CKeyboardWindows();

        //! \copydoc IKeyboard::setKeysToMonitor()
        virtual void setKeysToMonitor(const BlackMisc::Hardware::CKeyboardKeyList &keylist) override;

        //! \copydoc IKeyboard::selectKey()
        virtual void startCapture(bool ignoreNextKey = false) override;

        //! \copydoc IKeyboard::triggerKey()
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey key, bool isPressed) override;

        //! \brief Keyboard hook handle
        HHOOK keyboardHook() const { return m_keyboardHook; }

        //! \private
        void processKeyEvent(WPARAM vkCode, uint event);

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

    private:

        /*!
         * \brief Constructor
         * \param keySet
         * \param isFinished
         */
        void sendCaptureNotification(const BlackMisc::Hardware::CKeyboardKey &key, bool isFinished);

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


        BlackMisc::Hardware::CKeyboardKeyList m_listMonitoredKeys; //!< Registered keys
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        HHOOK m_keyboardHook;                   //!< Keyboard hook handle
        Mode m_mode;                            //!< Operation mode
    };
}

#endif // BLACKINPUT_KEYBOARD_WINDOWS_H
