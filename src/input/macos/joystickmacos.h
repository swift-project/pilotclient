// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_INPUT_JOYSTICKMACOS_H
#define SWIFT_INPUT_JOYSTICKMACOS_H

//! \file

#include <IOKit/hid/IOHIDManager.h>

#include <QHash>

#include "input/joystick.h"
#include "misc/input/joystickbutton.h"

namespace swift::input
{

    //! Joystick device
    class CJoystickDevice : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CJoystickDevice(QObject *parent = nullptr);

        //! Destructor
        virtual ~CJoystickDevice() override;

        //! Initialize device
        bool init(const IOHIDDeviceRef device);

        //! Get all available device buttons
        swift::misc::input::CJoystickButtonList getDeviceButtons() const;

        //! Return the native IOHIDDeviceRef
        IOHIDDeviceRef getNativeDevice() const { return m_deviceRef; }

    signals:
        //! Joystick button changed
        void buttonChanged(const swift::misc::input::CJoystickButton &joystickButton, bool isPressed);

    private:
        void processButtonEvent(IOHIDValueRef value);

        static void valueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value);

        QString m_deviceName = "unknown"; //!< Device name
        // IOHIDDeviceRef is owned by IOHIDManager. Do not release it.
        IOHIDDeviceRef m_deviceRef = nullptr;
        QHash<IOHIDElementRef, swift::misc::input::CJoystickButton> m_joystickDeviceInputs;
    };

    //! MacOS implemenation of IJoystick
    class CJoystickMacOS : public IJoystick
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CJoystickMacOS(CJoystickMacOS const &) = delete;

        //! Assignment operator
        CJoystickMacOS &operator=(CJoystickMacOS const &) = delete;

        //! Destructor
        virtual ~CJoystickMacOS() override;

        //! \copydoc swift::input::IJoystick::getAllAvailableJoystickButtons()
        virtual swift::misc::input::CJoystickButtonList getAllAvailableJoystickButtons() const override;

    protected:
        virtual bool init() override;

    private:
        friend class IJoystick;

        //! Destructor
        CJoystickMacOS(QObject *parent = nullptr);

        //! Add new joystick device
        void addJoystickDevice(const IOHIDDeviceRef device);

        //! Remove joystick device
        void removeJoystickDevice(const IOHIDDeviceRef device);

        void joystickButtonChanged(const swift::misc::input::CJoystickButton &joystickButton, bool isPressed);

        static void matchCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
        static void removeCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);

        IOHIDManagerRef m_hidManager = nullptr;
        QVector<CJoystickDevice *> m_joystickDevices; //!< Joystick devices

        swift::misc::input::CHotkeyCombination m_buttonCombination;
    };

} // namespace swift::input

#endif // SWIFT_INPUT_JOYSTICKMACOS_H
