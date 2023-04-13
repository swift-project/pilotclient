/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYBOARD_LINUX_H
#define BLACKINPUT_KEYBOARD_LINUX_H

#include "blackinput/keyboard.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QHash>
#include <QDir>
#include <QFileSystemWatcher>

#include <X11/X.h>
#include <X11/Xlib.h>

namespace BlackInput
{
    //! \brief Linux implemenation of IKeyboard using hook procedure
    class CKeyboardLinux : public IKeyboard
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CKeyboardLinux(CKeyboardLinux const &) = delete;

        //! Assignment operator
        CKeyboardLinux &operator=(CKeyboardLinux const &) = delete;

        //! Destructor
        virtual ~CKeyboardLinux() override;

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

        //! Constructor
        CKeyboardLinux(QObject *parent = nullptr);

        void addRawInputDevice(const QString &filePath);
        void keyEvent(int keyCode, bool isPressed);
        BlackMisc::Input::KeyCode convertToKey(int keyCode);
        bool isModifier(int keyCode);
        bool isMouseButton(int keyCode);

        BlackMisc::Input::CHotkeyCombination m_keyCombination; //!< Current status of pressed keys;

        QFileSystemWatcher *m_devInputWatcher; //!< Watches the device file system for input devices
        QHash<QString, QSharedPointer<QFile>> m_keyboardDevices; //!< All known input devices

        Display *m_display;
    };
}

#endif // guard
