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
#include "blackmisc/hwkeyboardkeylist.h"
#include <QHash>

class QFileSystemWatcher;
class QFile;

namespace BlackInput
{
    //! \brief Linux implemenation of IKeyboard using hook procedure
    //! \todo Change QHash to a CCollection object
    class CKeyboardLinux : public IKeyboard
    {
        Q_OBJECT

    public:

        //! \brief Copy Constructor
        CKeyboardLinux(CKeyboardLinux const&) = delete;

        //! \brief Assignment operator
        void operator=(CKeyboardLinux const&) = delete;

        //! \brief Destructor
        virtual ~CKeyboardLinux();

        //! Set the list of keys to monitor
        virtual void setKeysToMonitor(const BlackMisc::Hardware::CKeyboardKeyList &keylist) override;

        //! \copydoc IKeyboard::selectKey()
        virtual void startCapture(bool ignoreNextKey = false) override;

        //! \copydoc IKeyboard::triggerKey()
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey &key, bool isPressed) override;

    protected:

        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private slots:

        //! Changed directory to linux devices
        void deviceDirectoryChanged(const QString &);

        //! Device is ready to read new input
        void inputReadyRead(int);

    private:

        friend class IKeyboard;

        //! \brief Constructor
        CKeyboardLinux(QObject *parent = nullptr);

        /*!
         * \brief Constructor
         * \param keySet
         * \param isFinished
         */
        void sendCaptureNotification(const BlackMisc::Hardware::CKeyboardKey &key, bool isFinished);

        /*!
         * \brief Add new raw input device
         * \param filePath Path to device file
         */
        void addRawInputDevice(const QString &filePath);

        /*!
         * \brief Process new key event
         * \param virtualKeyCode
         * \param isPressed
         */
        void keyEvent(int virtualKeyCode, bool isPressed);

        BlackMisc::Hardware::CKeyboardKeyList m_listMonitoredKeys; //!< Registered keys
        BlackMisc::Hardware::CKeyboardKey m_pressedKey;    //!< Set of virtual keys pressed in the last cycle
        bool m_ignoreNextKey;                   //!< Is true if the next key needs to be ignored
        Mode m_mode;                            //!< Operation mode

        QFileSystemWatcher *m_devInputWatcher; //!< Watches the device file system for input devices
        QHash<QString, QFile *> m_hashInputDevices; //!< Hash map containing all known input devices
    };
}

#endif // BLACKINPUT_KEYBOARD_LINUX_H
