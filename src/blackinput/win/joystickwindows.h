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

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <dinput.h>

namespace BlackInput
{
    //! Joystick device data
    struct CJoystickDeviceData
    {
        GUID guidDevice; //!< Device GUID
        GUID guidProduct; //!< Product GUID
        QString deviceName; //!< Device name
        QString productName; //!< Product name
    };

    //! Joystick device input/button
    struct CJoystickDeviceInput
    {
        int m_number; //!< Input number
        int m_offset; //!< Input offset
        QString m_name; //!< Input name
    };

    //! Equal operator
    bool operator == (CJoystickDeviceData const &lhs, CJoystickDeviceData const &rhs);

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
        virtual ~CJoystickWindows();

    protected:

        //! Timer based updates
        virtual void timerEvent(QTimerEvent *event) override;

    private:

        friend class IJoystick;

        //! Constructor
        CJoystickWindows(QObject *parent = nullptr);

        //! Initialize DirectInput
        HRESULT initDirectInput();

        //! Enumerate all attached joystick devices
        HRESULT enumJoystickDevices();

        void filterJoystickDevices();

        //! Create a joystick device
        HRESULT createJoystickDevice();

        //! Poll the device buttons
        HRESULT pollDeviceState();

        //! Creates a hidden DI helper window
        int createHelperWindow();

        //! Update and signal button status to InputManager
        void updateAndSendButtonStatus(qint32 buttonIndex, bool isPressed);

        //! Add new joystick device
        void addJoystickDevice(const DIDEVICEINSTANCE *pdidInstance);

        //! Add new joystick input/button
        void addJoystickDeviceInput(const DIDEVICEOBJECTINSTANCE *dev);

        //! Joystick enumeration callback
        static BOOL CALLBACK enumJoysticksCallback(const DIDEVICEINSTANCE *
                pdidInstance, VOID *pContext);

        //! Joystick button enumeration callback
        static BOOL CALLBACK enumObjectsCallback(const DIDEVICEOBJECTINSTANCE *dev, LPVOID pvRef);

        // todo RW: Try to use QScopedPointer. So far I could not find out how to use it with
        // IDirectInput8::CreateDevice
        IDirectInput8 *m_directInput; //!< DirectInput object
        IDirectInputDevice8 *m_directInputDevice; //!< DirectInput device
        QVector<CJoystickDeviceData> m_availableJoystickDevices; //!< List of found and available joystick devices
        QVector<CJoystickDeviceInput> m_joystickDeviceInputs; //!< List of available device buttons

        BlackMisc::Input::CHotkeyCombination m_buttonCombination;

        static const WCHAR *m_helperWindowClassName; //!< Helper window class name
        static const WCHAR *m_helperWindowName; //!< Helper window name
        static ATOM m_helperWindowClass;
        static HWND m_helperWindow; //!< Helper window handle

    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICK_WINDOWS_H
