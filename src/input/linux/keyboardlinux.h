// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_KEYBOARD_LINUX_H
#define SWIFT_INPUT_KEYBOARD_LINUX_H

#include <QDir>
#include <QFileSystemWatcher>
#include <QHash>

#include "input/keyboard.h"
#include "misc/input/hotkeycombination.h"

// Explicitly including X11 last
#include <X11/X.h>
#include <X11/Xlib.h>

namespace swift::input
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
        ~CKeyboardLinux() override;

    protected:
        //! \copydoc IKeyboard::init()
        bool init() override;

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
        swift::misc::input::KeyCode convertToKey(int keyCode);
        bool isModifier(int keyCode);
        bool isMouseButton(int keyCode);

        swift::misc::input::CHotkeyCombination m_keyCombination; //!< Current status of pressed keys;

        QFileSystemWatcher *m_devInputWatcher; //!< Watches the device file system for input devices
        QHash<QString, QSharedPointer<QFile>> m_keyboardDevices; //!< All known input devices

        Display *m_display;
    };
} // namespace swift::input

#endif // SWIFT_INPUT_KEYBOARD_LINUX_H
