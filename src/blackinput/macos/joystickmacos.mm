/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystickmacos.h"
#include "macosinpututils.h"
#include "blackmisc/logmessage.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDUsageTables.h>

#include <array>

using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackInput
{
    CJoystickDevice::CJoystickDevice(QObject *parent) :
        QObject(parent)
    { }

    CJoystickDevice::~CJoystickDevice()
    { }

    bool CJoystickDevice::init(const IOHIDDeviceRef device)
    {
        m_deviceRef = device;

        CFTypeRef property = IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));
        m_deviceName = QString::fromCFString(static_cast<CFStringRef>(property));

        CLogMessage(static_cast<CJoystickMacOS *>(nullptr)).debug() << "Found joystick device" << m_deviceName;

        CFArrayRef elements =  IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);

        for (int i = 0;  i < CFArrayGetCount(elements);  i++)
        {
            IOHIDElementRef elementRef = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, i);
            if (CFGetTypeID(elementRef) != IOHIDElementGetTypeID()) { continue; }

            const IOHIDElementType type = IOHIDElementGetType(elementRef);
            if (type != kIOHIDElementTypeInput_Button) { continue; }

            const uint32_t page = IOHIDElementGetUsagePage(elementRef);

            if (page == kHIDPage_Button)
            {
                CLogMessage(static_cast<CJoystickMacOS *>(nullptr)).debug() << "Found joystick button " << m_joystickDeviceInputs.size();

                int number = m_joystickDeviceInputs.size();
                m_joystickDeviceInputs.insert(elementRef, { m_deviceName, number });
                IOHIDDeviceRegisterInputValueCallback(device, valueCallback, this);
            }
        }
        CFRelease(elements);

        // Filter devices with 0 buttons
        if (m_joystickDeviceInputs.isEmpty()) { return false; }

        CLogMessage(this).info(u"Created joystick device '%1' with %2 buttons") << m_deviceName << m_joystickDeviceInputs.size();
        return true;
    }

    CJoystickButtonList CJoystickDevice::getDeviceButtons() const
    {
        return CSequence<CJoystickButton>(m_joystickDeviceInputs.values());
    }

    void CJoystickDevice::processButtonEvent(IOHIDValueRef value)
    {
        IOHIDElementRef element = IOHIDValueGetElement(value);
        CJoystickButton joystickButton = m_joystickDeviceInputs.value(element);
        if (joystickButton.isValid())
        {
            bool isPressed = IOHIDValueGetIntegerValue(value) == 1;
            if (isPressed) { emit buttonChanged(joystickButton, true); }
            else { emit buttonChanged(joystickButton, false); }
        }
    }

    void CJoystickDevice::valueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value)
    {
        Q_UNUSED(result);
        Q_UNUSED(sender);
        CJoystickDevice *obj = static_cast<CJoystickDevice *>(context);
        obj->processButtonEvent(value);
    }

    CJoystickMacOS::CJoystickMacOS(QObject *parent) : IJoystick(parent)
    { }

    CJoystickMacOS::~CJoystickMacOS()
    {
        for (CJoystickDevice *d : m_joystickDevices)
        {
            delete d;
        }
        m_joystickDevices.clear();

        if (m_hidManager)
        {
            IOHIDManagerClose(m_hidManager, kIOHIDOptionsTypeNone);
            CFRelease(m_hidManager);
        }
    }

    CJoystickButtonList CJoystickMacOS::getAllAvailableJoystickButtons() const
    {
        CJoystickButtonList availableButtons;
        for (const CJoystickDevice *device : std::as_const(m_joystickDevices))
        {
            availableButtons.push_back(device->getDeviceButtons());
        }
        return availableButtons;
    }

    bool CJoystickMacOS::init()
    {
        m_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

        CFMutableArrayRef matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        if (!matchingArray)
        {
            CLogMessage(this).warning(u"Cocoa: Failed to create array");
            return false;
        }

        CFDictionaryRef matchingDict = CMacOSInputUtils::createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
        if (matchingDict)
        {
            CFArrayAppendValue(matchingArray, matchingDict);
            CFRelease(matchingDict);
        }

        matchingDict = CMacOSInputUtils::createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
        if (matchingDict)
        {
            CFArrayAppendValue(matchingArray, matchingDict);
            CFRelease(matchingDict);
        }

        matchingDict = CMacOSInputUtils::createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_MultiAxisController);
        if (matchingDict)
        {
            CFArrayAppendValue(matchingArray, matchingDict);
            CFRelease(matchingDict);
        }

        IOHIDManagerSetDeviceMatchingMultiple(m_hidManager, matchingArray);
        CFRelease(matchingArray);

        IOHIDManagerRegisterDeviceMatchingCallback(m_hidManager, matchCallback, this);
        IOHIDManagerRegisterDeviceRemovalCallback(m_hidManager, removeCallback, this);
        IOHIDManagerScheduleWithRunLoop(m_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        return IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone) == kIOReturnSuccess;
    }

    void CJoystickMacOS::addJoystickDevice(const IOHIDDeviceRef device)
    {
        for (const CJoystickDevice *d : m_joystickDevices)
        {
            if (d->getNativeDevice() == device) { return; }
        }

        CJoystickDevice *joystickDevice = new CJoystickDevice(this);
        bool success = joystickDevice->init(device);
        if (success)
        {
            connect(joystickDevice, &CJoystickDevice::buttonChanged, this, &CJoystickMacOS::joystickButtonChanged);
            m_joystickDevices.push_back(joystickDevice);
        }
        else
        {
            delete joystickDevice;
        }
    }

    void CJoystickMacOS::removeJoystickDevice(const IOHIDDeviceRef device)
    {
        for (auto it = m_joystickDevices.begin(); it != m_joystickDevices.end();)
        {
            CJoystickDevice *d = *it;
            if (d->getNativeDevice() == device)
            {
                delete d;
                it = m_joystickDevices.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void CJoystickMacOS::joystickButtonChanged(const CJoystickButton &joystickButton, bool isPressed)
    {
        CHotkeyCombination oldCombination(m_buttonCombination);
        if (isPressed) { m_buttonCombination.addJoystickButton(joystickButton); }
        else { m_buttonCombination.removeJoystickButton(joystickButton); }

        if (oldCombination != m_buttonCombination)
        {
            emit buttonCombinationChanged(m_buttonCombination);
        }
    }

    void CJoystickMacOS::matchCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device)
    {
        Q_UNUSED(result);
        Q_UNUSED(sender);
        CJoystickMacOS *obj = static_cast<CJoystickMacOS *>(context);
        obj->addJoystickDevice(device);
    }

    void CJoystickMacOS::removeCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device)
    {
        Q_UNUSED(result);
        Q_UNUSED(sender);
        CJoystickMacOS *obj = static_cast<CJoystickMacOS *>(context);
        obj->removeJoystickDevice(device);
    }

} // namespace BlackInput
