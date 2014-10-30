/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_MAC_H
#define BLACKINPUT_KEYBOARD_MAC_H

#include "blackinput/keyboard.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include <QHash>

namespace BlackInput
{
    //! \brief Linux implemenation of IKeyboard using hook procedure
    //! \todo Change QHash to a CCollection object
    class CKeyboardMac : public IKeyboard
    {
        Q_OBJECT

    public:

        //! \brief Copy Constructor
        CKeyboardMac(CKeyboardMac const&) = delete;

        //! \brief Assignment operator
        CKeyboardMac &operator=(CKeyboardMac const&) = delete;

        //! \brief Destructor
        virtual ~CKeyboardMac();

        //! Set the list of keys to monitor
        virtual void setKeysToMonitor(const BlackMisc::Hardware::CKeyboardKeyList &keylist) override;

        //! \copydoc IKeyboard::selectKey()
        virtual void startCapture(bool ignoreNextKey = false) override;

        //! \copydoc IKeyboard::triggerKey()
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey &key, bool isPressed) override;

    protected:

        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private:

        friend class IKeyboard;

        //! Constructor
        CKeyboardMac(QObject *parent = nullptr);

        /*!
         * \brief Constructor
         * \param keySet
         * \param isFinished
         */
        void sendCaptureNotification(const BlackMisc::Hardware::CKeyboardKey &key, bool isFinished);

        BlackMisc::Hardware::CKeyboardKeyList m_listMonitoredKeys; //!< Registered keys
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        Mode m_mode;                            //!< Operation mode
    };
}

#endif // BLACKINPUT_KEYBOARD_MAC_H
