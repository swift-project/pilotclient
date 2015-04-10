/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_H
#define BLACKINPUT_KEYBOARD_H

#include "blackinputexport.h"
#include "blackmisc/hardware/keyboardkeylist.h"
#include <QMultiMap>
#include <QObject>
#include <QPointer>
#include <functional>

namespace BlackInput
{
    /*!
     * Abstract interface for native keyboard handling.
     * \todo Add implementation for Linux and OSX.
     */
    class BLACKINPUT_EXPORT IKeyboard : public QObject
    {
        Q_OBJECT
    public:

        //! Operation mode
        enum Mode
        {
            Mode_Nominal,
            Mode_Capture
        };

        //! Constructor
        IKeyboard(QObject *parent = nullptr);

        //! Destructor
        virtual ~IKeyboard() {}

        //! Set the list of keys to monitor
        virtual void setKeysToMonitor(const BlackMisc::Hardware::CKeyboardKeyList &keylist) = 0;

        /*!
         * Select a key combination as hotkey. This method returns immediatly.
         *        Listen for signals keySelectionChanged and keySelectionFinished
         *        to retrieve the user input.
         * \param ignoreNextKey
         *        Set to true, if you want to ignore the first key,
         *        e.g. [ENTER] in case you are running from command line.
         */
        virtual void startCapture(bool ignoreNextKey) = 0;

        /*!
         * Triggers a key event manually and calls the registered functions.
         * \param key
         * \param isPressed
         */
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey &key, bool isPressed) = 0;

        //! Creates a native keyboard handler object
        static IKeyboard *getInstance();

    signals:

        /*!
         * Key selection has changed, but is not finished yet.
         * \param key
         */
        void keySelectionChanged(BlackMisc::Hardware::CKeyboardKey key);

        /*!
         * Key selection has finished.
         * \param key
         */
        void keySelectionFinished(BlackMisc::Hardware::CKeyboardKey key);

        //! Key down
        void keyDown(const BlackMisc::Hardware::CKeyboardKey &);

        //! Key up
        void keyUp(const BlackMisc::Hardware::CKeyboardKey &);

    protected:

        /*!
         * Initializes the platform keyboard device
         */
        virtual bool init() = 0;

    private:

        static IKeyboard *m_instance;
    };
}

#endif // guard
