/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#    define NOMINMAX
#endif

#include <dinput.h>

namespace BlackInput
{
    //! Shared IDirectInput8 ptr
    using DirectInput8Ptr = std::shared_ptr<IDirectInput8>;

    //! Joystick device input/button
    struct CJoystickDeviceInput
    {
        int m_offset; //!< Input offset
        BlackMisc::Input::CJoystickButton m_button; //!< Joystick button
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

        //! Get all available device buttons
        BlackMisc::Input::CJoystickButtonList getDeviceButtons() const;

        //! Get device GUID
        GUID getDeviceGuid() const { return m_guidDevice; }

    signals:
        //! Joystick button changed
        void buttonChanged(const BlackMisc::Input::CJoystickButton &joystickButton, bool isPressed);

        //! Connection to joystick lost. Probably unplugged.
        void connectionLost(const GUID &guid);

    protected:
        //! Timer based updates
        virtual void timerEvent(QTimerEvent *event) override;

    private:
        friend bool operator==(const CJoystickDevice &lhs, const CJoystickDevice &rhs);

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

        //! Code as tring
        static QString hrString(HRESULT hr);

        //! Joystick button enumeration callback
        static BOOL CALLBACK enumObjectsCallback(const DIDEVICEOBJECTINSTANCE *dev, LPVOID pvRef);

        GUID m_guidDevice; //!< Device GUID
        GUID m_guidProduct; //!< Product GUID
        QString m_deviceName; //!< Device name
        QString m_productName; //!< Product name
        DirectInput8Ptr m_directInput;
        DirectInputDevice8Ptr m_directInputDevice;
        QVector<CJoystickDeviceInput> m_joystickDeviceInputs;
    };

    //! Equal operator
    bool operator==(CJoystickDevice const &lhs, CJoystickDevice const &rhs);

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

        //! \copydoc BlackInput::IJoystick::getAllAvailableJoystickButtons()
        virtual BlackMisc::Input::CJoystickButtonList getAllAvailableJoystickButtons() const override;

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

        //! Request USB device notifications sent to our helper window.
        //! This is required for joystick hotplug support
        void requestDeviceNotification();

        //! Destroys a hidden DI helper window
        void destroyHelperWindow();

        //! Add new joystick device
        void addJoystickDevice(const DIDEVICEINSTANCE *pdidInstance);

        //! Remove joystick device
        void removeJoystickDevice(const GUID &guid);

        //! Is joystick instance already added?
        bool isJoystickAlreadyAdded(const DIDEVICEINSTANCE *pdidInstance) const;

        void joystickButtonChanged(const BlackMisc::Input::CJoystickButton &joystickButton, bool isPressed);

        static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        //! Joystick enumeration callback
        static BOOL CALLBACK enumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);

        HWND helperWindow = nullptr;
        HDEVNOTIFY hDevNotify = nullptr;

        const TCHAR *helperWindowClassName = TEXT("HelperWindow");
        const TCHAR *helperWindowName = TEXT("JoystickCatcherWindow");

        bool m_coInitializeSucceeded = false;
        DirectInput8Ptr m_directInput; //!< DirectInput object
        QVector<CJoystickDevice *> m_joystickDevices; //!< Joystick devices

        BlackMisc::Input::CHotkeyCombination m_buttonCombination;
    };
} // ns

#endif // guard
