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
    const TCHAR *CJoystickWindows::m_helperWindowClassName = TEXT("HelperWindow");
    const TCHAR *CJoystickWindows::m_helperWindowName = TEXT("JoystickCatcherWindow");
    ATOM CJoystickWindows::m_helperWindowClass = 0;
    HWND CJoystickWindows::m_helperWindow = nullptr;

    CJoystickWindows::CJoystickWindows(QObject *parent) : IJoystick(parent)
    {
        // Initialize COM
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        this->initDirectInput();
        this->enumJoystickDevices();
        this->filterJoystickDevices();
        if (!m_availableJoystickDevices.isEmpty()) { createJoystickDevice(); }
    }

    CJoystickWindows::~CJoystickWindows()
    {
        // release device before input
        if (m_directInputDevice)
        {
            m_directInputDevice->Release();
            m_directInputDevice = nullptr;
        }
        if (m_directInput)
        {
            m_directInput->Release();
            m_directInput = nullptr;
        }

        CoUninitialize();
    }

    void CJoystickWindows::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event);
        this->pollDeviceState();
    }

    HRESULT CJoystickWindows::initDirectInput()
    {
        HRESULT hr = CoCreateInstance(CLSID_DirectInput8, nullptr, CLSCTX_INPROC_SERVER, IID_IDirectInput8, (LPVOID *)&m_directInput);
        if (FAILED(hr))
        {
            CLogMessage(this).error("Cannot create instance %1") << GetLastError();
            return hr;
        }

        HINSTANCE instance = GetModuleHandle(nullptr);
        if (instance == nullptr)
        {
            CLogMessage(this).error("GetModuleHandle() failed with error code: %1") << GetLastError();
            return E_FAIL;
        }

        if (FAILED(hr = m_directInput->Initialize(instance, DIRECTINPUT_VERSION)))
        {
            CLogMessage(this).error("Direct input init failed");
            return hr;
        }
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
        if (FAILED(hr = m_directInput->EnumDevices(DI8DEVTYPE_JOYSTICK, enumJoysticksCallback, this, DIEDFL_ATTACHEDONLY)))
        {
            CLogMessage(this).error("Error reading joystick devices");
            return hr;
        }

        if (m_availableJoystickDevices.isEmpty())
        {
            CLogMessage(this).info("No joystick device found");
        }
        return hr;
    }

    void CJoystickWindows::filterJoystickDevices()
    {
        IDirectInputDevice8 *directInputDevice = nullptr;
        DIDEVCAPS deviceCaps;
        deviceCaps.dwSize = sizeof(DIDEVCAPS);
        HRESULT hr = S_OK;
        for (auto i = m_availableJoystickDevices.begin(); i != m_availableJoystickDevices.end();)
        {
            // Create device
            hr = m_directInput->CreateDevice(i->guidDevice, &directInputDevice, nullptr);
            if (FAILED(hr))
            {
                i = m_availableJoystickDevices.erase(i);
                continue;
            }

            hr = directInputDevice->GetCapabilities(&deviceCaps);
            if (FAILED(hr))
            {
                i = m_availableJoystickDevices.erase(i);
                continue;
            }

            // Filter devices with 0 buttons
            if (deviceCaps.dwButtons == 0)
            {
                i = m_availableJoystickDevices.erase(i);
                continue;
            }

            if (directInputDevice)
            {
                directInputDevice->Release();
                directInputDevice = nullptr;
            }

            ++i;
        }
    }

    HRESULT CJoystickWindows::createJoystickDevice()
    {
        HRESULT hr = S_OK;

        // Check if device list is empty first
        if (m_availableJoystickDevices.isEmpty()) { return E_FAIL; }

        // FIXME: Take the first device with number of buttons > 0
        // For the future, the user should be able to choose which device
        // he wants to use.
        const CJoystickDeviceData &deviceData = m_availableJoystickDevices.constFirst();

        // Create device
        if (FAILED(hr = m_directInput->CreateDevice(deviceData.guidDevice, &m_directInputDevice, nullptr)))
        {
            // FIXME: print error message
            return hr;
        }

        createHelperWindow();

        // Set cooperative level
        if (FAILED(hr = m_directInputDevice->SetCooperativeLevel(m_helperWindow, DISCL_NONEXCLUSIVE |
                        DISCL_BACKGROUND)))
        {
            // FIXME: print error message
            return hr;
        }

        // Set data format to c_dfDIJoystick2
        if (FAILED(hr = m_directInputDevice->SetDataFormat(&c_dfDIJoystick2)))
        {
            // FIXME: print error message
            return hr;
        }

        DIDEVCAPS deviceCaps;
        deviceCaps.dwSize = sizeof(DIDEVCAPS);
        // Get device capabilities - we are interested in the number of buttons.
        if (FAILED(hr = m_directInputDevice->GetCapabilities(&deviceCaps)))
        {
            // FIXME: print error message
            return hr;
        }

        m_joystickDeviceInputs.clear();
        if (FAILED(hr = m_directInputDevice->EnumObjects(enumObjectsCallback, this, DIDFT_BUTTON)))
        {
            // FIXME: print error message
            return hr;
        }

        CLogMessage(this).info("Created joystick device '%1' with %2 buttons") << deviceData.deviceName << deviceCaps.dwButtons;

        startTimer(50);
        return hr;
    }

    int CJoystickWindows::createHelperWindow()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASS wce;

        /* Make sure window isn't created twice. */
        if (m_helperWindow != nullptr)
        {
            return 0;
        }

        /* Create the class. */
        ZeroMemory(&wce, sizeof(WNDCLASS));
        wce.lpfnWndProc = DefWindowProc;
        wce.lpszClassName = (LPCWSTR) m_helperWindowClassName;
        wce.hInstance = hInstance;

        /* Register the class. */
        m_helperWindowClass = RegisterClass(&wce);
        if (m_helperWindowClass == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        {
            return -1;
        }

        /* Create the window. */
        m_helperWindow = CreateWindowEx(0, m_helperWindowClassName,
                                        m_helperWindowName,
                                        WS_OVERLAPPED, CW_USEDEFAULT,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        CW_USEDEFAULT, HWND_MESSAGE, nullptr,
                                        hInstance, nullptr);
        if (m_helperWindow == nullptr)
        {
            UnregisterClass(m_helperWindowClassName, hInstance);
            return -1;
        }

        return 0;
    }

    void CJoystickWindows::updateAndSendButtonStatus(qint32 buttonIndex, bool isPressed)
    {
        BlackMisc::Input::CHotkeyCombination oldCombination(m_buttonCombination);
        if (isPressed) { m_buttonCombination.addJoystickButton(buttonIndex); }
        else { m_buttonCombination.removeJoystickButton(buttonIndex); }

        if (oldCombination != m_buttonCombination)
        {
            emit buttonCombinationChanged(m_buttonCombination);
        }
    }

    void CJoystickWindows::addJoystickDevice(const DIDEVICEINSTANCE *pdidInstance)
    {
        CJoystickDeviceData deviceData;
        deviceData.deviceName = QString::fromWCharArray(pdidInstance->tszInstanceName);
        deviceData.productName = QString::fromWCharArray(pdidInstance->tszProductName);
        deviceData.guidDevice = pdidInstance->guidInstance;
        deviceData.guidProduct = pdidInstance->guidProduct;

        if (!m_availableJoystickDevices.contains(deviceData)) m_availableJoystickDevices.push_back(deviceData);
    }

    void CJoystickWindows::addJoystickDeviceInput(const DIDEVICEOBJECTINSTANCE *dev)
    {
        CJoystickDeviceInput deviceInput;
        deviceInput.m_number = m_joystickDeviceInputs.size();
        deviceInput.m_offset = DIJOFS_BUTTON(deviceInput.m_number);
        deviceInput.m_name = QString::fromWCharArray(dev->tszName);

        m_joystickDeviceInputs.append(deviceInput);
    }

    HRESULT CJoystickWindows::pollDeviceState()
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
            qint32 buttonIndex = input.m_offset - DIJOFS_BUTTON0;
            updateAndSendButtonStatus(buttonIndex, state.rgbButtons[buttonIndex] & 0x80);
        }

        return hr;
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

    BOOL CALLBACK CJoystickWindows::enumObjectsCallback(const DIDEVICEOBJECTINSTANCE *dev, LPVOID pvRef)
    {
        CJoystickWindows *joystick = static_cast<CJoystickWindows *>(pvRef);

        // Make sure we only got GUID_Button types
        if (dev->guidType != GUID_Button) return DIENUM_CONTINUE;

        joystick->addJoystickDeviceInput(dev);
        CLogMessage(static_cast<CJoystickWindows *>(nullptr)).debug() << "Found joystick button" << QString::fromWCharArray(dev->tszName);

        return DIENUM_CONTINUE;
    }

    bool operator == (CJoystickDeviceData const &lhs, CJoystickDeviceData const &rhs)
    {
        return lhs.guidDevice == rhs.guidDevice &&
               lhs.guidProduct == rhs.guidProduct &&
               lhs.deviceName == rhs.deviceName &&
               lhs.productName == rhs.productName;
    }

    bool operator == (CJoystickDeviceInput const &lhs, CJoystickDeviceInput const &rhs)
    {
        return lhs.m_number == rhs.m_number &&
               lhs.m_offset == rhs.m_offset &&
               lhs.m_name == rhs.m_name;
    }

} // namespace BlackInput
