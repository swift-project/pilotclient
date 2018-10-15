/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystickwindows.h"
#include "blackmisc/logmessage.h"

// Qt5 defines UNICODE, hence we can expect an wchar_t strings.
// If it fails to compile, because of char/wchar_t errors, you are most likely
// using ascii functions of WINAPI. To fix it, introduce #ifdef UNICODE and add char
// handling in the second branch.

using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackInput
{
    CJoystickDevice::CJoystickDevice(DirectInput8Ptr directInputPtr, const DIDEVICEINSTANCE *pdidInstance, QObject *parent)
        : QObject(parent),
          m_directInput(directInputPtr)
    {
        m_deviceName = QString::fromWCharArray(pdidInstance->tszInstanceName);
        m_productName = QString::fromWCharArray(pdidInstance->tszProductName);
        m_guidDevice = pdidInstance->guidInstance;
        m_guidProduct = pdidInstance->guidProduct;
    }

    bool CJoystickDevice::init(HWND helperWindow)
    {
        HRESULT hr;
        // Create device
        {
            IDirectInputDevice8 *diDevice = nullptr;
            if (FAILED(hr = m_directInput->CreateDevice(m_guidDevice, &diDevice, nullptr)))
            {
                CLogMessage(this).warning("IDirectInput8::CreateDevice failed: ") << hr;
                return false;
            }
            m_directInputDevice.reset(diDevice);
        }

        // Set cooperative level
        if (!helperWindow) { return false; }
        if (FAILED(hr = m_directInputDevice->SetCooperativeLevel(helperWindow, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
        {
            CLogMessage(this).warning("IDirectInputDevice8::SetCooperativeLevel failed: ") << hr;
            return false;
        }

        // Set data format to c_dfDIJoystick2
        if (FAILED(hr = m_directInputDevice->SetDataFormat(&c_dfDIJoystick2)))
        {
            CLogMessage(this).warning("IDirectInputDevice8::SetDataFormat failed: ") << hr;
            return false;
        }

        DIDEVCAPS deviceCaps;
        deviceCaps.dwSize = sizeof(DIDEVCAPS);
        // Get device capabilities - we are interested in the number of buttons.
        if (FAILED(hr = m_directInputDevice->GetCapabilities(&deviceCaps)))
        {
            CLogMessage(this).warning("IDirectInputDevice8::GetCapabilities failed: ") << hr;
            return false;
        }

        // Filter devices with 0 buttons
        if (deviceCaps.dwButtons == 0) { return false; }

        if (FAILED(hr = m_directInputDevice->EnumObjects(enumObjectsCallback, this, DIDFT_BUTTON)))
        {
            CLogMessage(this).warning("IDirectInputDevice8::EnumObjects failed: ") << hr;
            return false;
        }

        CLogMessage(this).info("Created joystick device '%1' with %2 buttons") << m_deviceName << deviceCaps.dwButtons;
        this->startTimer(50);
        return true;
    }

    void CJoystickDevice::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event);
        pollDeviceState();
    }

    HRESULT CJoystickDevice::pollDeviceState()
    {
        DIJOYSTATE2 state;
        HRESULT hr = S_OK;

        if (FAILED(hr = m_directInputDevice->Poll()))
        {
            if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
            {
                m_directInputDevice->Acquire();
                if (FAILED(hr = m_directInputDevice->Poll()))
                {
                    CLogMessage(this).warning("DirectInput error code: ") << hr;
                    return hr;
                }
            }
        }

        if (FAILED(hr = m_directInputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &state)))
        {
            if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
            {
                m_directInputDevice->Acquire();
                if (FAILED(hr = m_directInputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &state)))
                {
                    CLogMessage(this).warning("DirectInput error code: ") << hr;
                    return hr;
                }
            }
        }

        for (CJoystickDeviceInput input : m_joystickDeviceInputs)
        {
            const qint32 buttonIndex = input.m_offset - DIJOFS_BUTTON0;
            bool isPressed = state.rgbButtons[buttonIndex] & 0x80;

            if (isPressed) { emit buttonChanged(m_deviceName, buttonIndex, true); }
            else { emit buttonChanged(m_deviceName, buttonIndex, false); }

        }
        return hr;
    }

    BOOL CALLBACK CJoystickDevice::enumObjectsCallback(const DIDEVICEOBJECTINSTANCE *dev, LPVOID pvRef)
    {
        CJoystickDevice *joystickDevice = static_cast<CJoystickDevice *>(pvRef);

        // Make sure we only got GUID_Button types
        if (dev->guidType != GUID_Button) return DIENUM_CONTINUE;

        CJoystickDeviceInput deviceInput;
        deviceInput.m_number = joystickDevice->m_joystickDeviceInputs.size();
        deviceInput.m_offset = DIJOFS_BUTTON(deviceInput.m_number);
        deviceInput.m_name = QString::fromWCharArray(dev->tszName);

        joystickDevice->m_joystickDeviceInputs.append(deviceInput);

        CLogMessage(static_cast<CJoystickWindows *>(nullptr)).debug() << "Found joystick button" << QString::fromWCharArray(dev->tszName);

        return DIENUM_CONTINUE;
    }

    CJoystickWindows::CJoystickWindows(QObject *parent) : IJoystick(parent)
    {
        // Initialize COM
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        this->createHelperWindow();

        if (helperWindow)
        {
            this->initDirectInput();
            this->enumJoystickDevices();
        }
    }

    CJoystickWindows::~CJoystickWindows()
    {
        m_joystickDevices.clear();
        m_directInput.reset();
        CoUninitialize();
        destroyHelperWindow();
    }

    void ReleaseDirectInput(IDirectInput8 *obj)
    {
        Q_UNUSED(obj);
        //! \todo temp workaround for crash when shutting down T391
        // if (obj) { obj->Release(); }
    }

    HRESULT CJoystickWindows::initDirectInput()
    {
        IDirectInput8 *directInput = nullptr;
        // HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID *>(&directInput), nullptr);
        HRESULT hr = CoCreateInstance(CLSID_DirectInput8, nullptr, CLSCTX_INPROC_SERVER, IID_IDirectInput8, reinterpret_cast<LPVOID *>(&directInput));
        if (FAILED(hr)) { return hr; }
        m_directInput = DirectInput8Ptr(directInput, ReleaseDirectInput);

        HINSTANCE instance = GetModuleHandle(nullptr);
        hr = m_directInput->Initialize(instance, DIRECTINPUT_VERSION);
        return hr;
    }

    HRESULT CJoystickWindows::enumJoystickDevices()
    {
        if (!m_directInput)
        {
            CLogMessage(this).warning("No direct input");
            return E_FAIL;
        }

        HRESULT hr;
        if (FAILED(hr = m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoysticksCallback, this, DIEDFL_ATTACHEDONLY)))
        {
            CLogMessage(this).error("Error reading joystick devices");
            return hr;
        }

        if (m_joystickDevices.empty())
        {
            CLogMessage(this).info("No joystick device found");
        }
        return hr;
    }

    int CJoystickWindows::createHelperWindow()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASS wce;

        // Make sure window isn't created twice
        if (helperWindow != nullptr)
        {
            return 0;
        }

        // Create the class
        ZeroMemory(&wce, sizeof(WNDCLASS));
        wce.lpfnWndProc = DefWindowProc;
        wce.lpszClassName = (LPCWSTR) helperWindowClassName;
        wce.hInstance = hInstance;

        /* Register the class. */
        helperWindowClass = RegisterClass(&wce);
        if (helperWindowClass == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        {
            return -1;
        }

        /* Create the window. */
        helperWindow = CreateWindowEx(0, helperWindowClassName,
                                      helperWindowName,
                                      WS_OVERLAPPED, CW_USEDEFAULT,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT, HWND_MESSAGE, nullptr,
                                      hInstance, nullptr);
        if (helperWindow == nullptr)
        {
            UnregisterClass(helperWindowClassName, hInstance);
            return -1;
        }

        return 0;
    }

    void CJoystickWindows::destroyHelperWindow()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        if (helperWindow == nullptr) { return; }

        DestroyWindow(helperWindow);
        helperWindow = nullptr;

        UnregisterClass(helperWindowClassName, hInstance);
        helperWindowClass = 0;
    }

    void CJoystickWindows::addJoystickDevice(const DIDEVICEINSTANCE *pdidInstance)
    {
        CJoystickDevice *device = new CJoystickDevice(m_directInput, pdidInstance, this);
        bool success = device->init(helperWindow);
        if (success)
        {
            connect(device, &CJoystickDevice::buttonChanged, this, &CJoystickWindows::joystickButtonChanged);
            m_joystickDevices.push_back(device);
        }
        else
        {
            device->deleteLater();
        }
    }

    void CJoystickWindows::joystickButtonChanged(const QString &name, int index, bool isPressed)
    {
        BlackMisc::Input::CHotkeyCombination oldCombination(m_buttonCombination);
        if (isPressed) { m_buttonCombination.addJoystickButton({name, index}); }
        else { m_buttonCombination.removeJoystickButton({name, index}); }

        if (oldCombination != m_buttonCombination)
        {
            emit buttonCombinationChanged(m_buttonCombination);
        }
    }

    BOOL CALLBACK CJoystickWindows::enumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
    {
        CJoystickWindows *obj = static_cast<CJoystickWindows *>(pContext);

        /* ignore XInput devices here, keep going. */
        //if (isXInputDevice( &pdidInstance->guidProduct )) return DIENUM_CONTINUE;

        obj->addJoystickDevice(pdidInstance);
        CLogMessage(static_cast<CJoystickWindows *>(nullptr)).debug() << "Found joystick device" << QString::fromWCharArray(pdidInstance->tszInstanceName);
        return true;
    }

    bool operator == (const CJoystickDevice &lhs, const CJoystickDevice &rhs)
    {
        return lhs.m_guidDevice == rhs.m_guidDevice &&
               lhs.m_guidProduct == rhs.m_guidProduct &&
               lhs.m_deviceName == rhs.m_deviceName &&
               lhs.m_productName == rhs.m_productName;
    }

    bool operator == (CJoystickDeviceInput const &lhs, CJoystickDeviceInput const &rhs)
    {
        return lhs.m_number == rhs.m_number &&
               lhs.m_offset == rhs.m_offset &&
               lhs.m_name == rhs.m_name;
    }
} // ns
