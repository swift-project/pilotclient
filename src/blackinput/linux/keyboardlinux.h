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
#include "blackmisc/input/hotkeycombination.h"
#include <QHash>

#include <X11/X.h>
#include <X11/Xlib.h>

class QFileSystemWatcher;
class QFile;

namespace BlackInput
{
    //! \brief Linux implemenation of IKeyboard using hook procedure
    class CKeyboardLinux : public IKeyboard
    {
        Q_OBJECT

    public:

        //! \brief Copy Constructor
        CKeyboardLinux(CKeyboardLinux const &) = delete;

        //! \brief Assignment operator
        CKeyboardLinux &operator=(CKeyboardLinux const &) = delete;

        //! \brief Destructor
        virtual ~CKeyboardLinux();

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

        void addRawInputDevice(const QString &filePath);
        void keyEvent(int keyCode, bool isPressed);
        BlackMisc::Input::KeyCode convertToKey(int keyCode);
        bool isModifier(int keyCode);
        bool isMouseButton(int keyCode);

        BlackMisc::Input::CHotkeyCombination m_keyCombination;    //!< Current status of pressed keys;

        QFileSystemWatcher *m_devInputWatcher; //!< Watches the device file system for input devices
        QHash<QString, QSharedPointer<QFile>> m_keyboardDevices; //!< All known input devices

        Display *m_display;
    };
}

#endif // BLACKINPUT_KEYBOARD_LINUX_H
