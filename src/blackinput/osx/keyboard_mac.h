/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYBOARD_MAC_H
#define BLACKINPUT_KEYBOARD_MAC_H

#include "blackinput/keyboard.h"
#include "blackmisc/hardware/keyboardkeylist.h"
#include <QHash>

class __CGEvent;
typedef __CGEvent* CGEventRef;

typedef unsigned int CGEventType;

class __CGEventTapProxy;
typedef __CGEventTapProxy* CGEventTapProxy;

namespace BlackInput
{
    //! \brief Mac OSX implemenation of IKeyboard using hook procedure
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

        virtual void processKeyEvent(CGEventType type, CGEventRef event);

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

        static CGEventRef myCGEventCallback(CGEventTapProxy proxy,
                                     CGEventType type,
                                     CGEventRef event,
                                     void *refcon);

        BlackMisc::Hardware::CKeyboardKeyList m_listMonitoredKeys; //!< Registered keys
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        Mode m_mode;                            //!< Operation mode
    };
}

#endif // BLACKINPUT_KEYBOARD_MAC_H
