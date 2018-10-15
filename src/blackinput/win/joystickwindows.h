/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKINPUT_JOYSTICKWINDOWS_H
#define BLACKINPUT_JOYSTICKWINDOWS_H

//! \file

#include "blackinput/blackinputexport.h"
#include "blackinput/joystick.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/collection.h"
#include <QVector>
#include <memory>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <dinput.h>

namespace BlackInput
{
    //! Shared IDirectInput8 ptr
    using DirectInput8Ptr = std::shared_ptr<IDirectInput8>;

    //! Joystick device input/button
    struct CJoystickDeviceInput
    {
        int m_number;   //!< Input number
        int m_offset;   //!< Input offset
        QString m_name; //!< Input name
    };

    //! Joystick device
    class CJoystickDevice : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CJoystickDevice(DirectInput8Ptr directInputPtr, const DIDEVICEINSTANCE *pdidInstance, QObject *parent = nullptr);

        //! Initialize DirectInput device
        bool init(HWND helperWindow);

    signals:
        //! Joystick button changed
        void buttonChanged(const QString &name, int index, bool isPressed);

    protected:
        //! Timer based updates
        virtual void timerEvent(QTimerEvent *event) override;

    private:
        friend bool operator == (const CJoystickDevice &lhs, const CJoystickDevice &rhs);

        struct DirectInputDevice8Deleter
        {
            void operator()(IDirectInputDevice8 *obj)
            {
                if (obj)
                {
                    obj->Unacquire();
                    obj->Release();
                }
            }
        };

        using DirectInputDevice8Ptr = std::unique_ptr<IDirectInputDevice8, DirectInputDevice8Deleter>;

        //! Poll the device buttons
        HRESULT pollDeviceState();

        //! Joystick button enumeration callback
        static BOOL CALLBACK enumObjectsCallback(const DIDEVICEOBJECTINSTANCE *dev, LPVOID pvRef);

        GUID m_guidDevice;     //!< Device GUID
        GUID m_guidProduct;    //!< Product GUID
        QString m_deviceName;  //!< Device name
        QString m_productName; //!< Product name
        DirectInput8Ptr m_directInput;
        DirectInputDevice8Ptr m_directInputDevice;
        QVector<CJoystickDeviceInput> m_joystickDeviceInputs;
    };

    //! Equal operator
    bool operator == (CJoystickDevice const &lhs, CJoystickDevice const &rhs);

    //! Windows implemenation of IJoystick with DirectInput
    class BLACKINPUT_EXPORT CJoystickWindows : public IJoystick
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CJoystickWindows(CJoystickWindows const &) = delete;

        //! Assignment operator
        CJoystickWindows &operator=(CJoystickWindows const &) = delete;

        //! \brief Destructor
        virtual ~CJoystickWindows() override;

    private:
        friend class IJoystick;

        //! Constructor
        CJoystickWindows(QObject *parent = nullptr);

        //! Initialize DirectInput
        HRESULT initDirectInput();

        //! Enumerate all attached joystick devices
        HRESULT enumJoystickDevices();

        //! Creates a hidden DI helper window
        int createHelperWindow();

        //! Destroys a hidden DI helper window
        void destroyHelperWindow();

        //! Add new joystick device
        void addJoystickDevice(const DIDEVICEINSTANCE *pdidInstance);

        void joystickButtonChanged(const QString &name, int index, bool isPressed);

        //! Joystick enumeration callback
        static BOOL CALLBACK enumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);

        ATOM helperWindowClass = 0;
        HWND helperWindow = nullptr;

        const TCHAR *helperWindowClassName = TEXT("HelperWindow");
        const TCHAR *helperWindowName = TEXT("JoystickCatcherWindow");

        bool m_coInitializeSucceeded = false;
        DirectInput8Ptr m_directInput;                 //!< DirectInput object
        QVector<CJoystickDevice *> m_joystickDevices;  //!< Joystick devices

        BlackMisc::Input::CHotkeyCombination m_buttonCombination;
    };
} // ns

#endif // guard
