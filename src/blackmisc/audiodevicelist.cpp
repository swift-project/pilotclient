/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "audiodevicelist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Audio
    {
        /*
         * Default constructor
         */
        CAudioDeviceList::CAudioDeviceList() { }

        /*
         * Construct from base class object
         */
        CAudioDeviceList::CAudioDeviceList(const CSequence &other) :
            CSequence(other)
        { }

        /*
         * Output devices
         */
        CAudioDeviceList CAudioDeviceList::getOutputDevices() const
        {
            return this->findBy(&CAudioDevice::getType, CAudioDevice::OutputDevice);
        }

        /*
         * Output devices
         */
        CAudioDeviceList CAudioDeviceList::getInputDevices() const
        {
            return this->findBy(&CAudioDevice::getType, CAudioDevice::InputDevice);
        }

        /*
         * Count as of type
         */
        int CAudioDeviceList::count(CAudioDevice::DeviceType type) const
        {
            return std::count_if(this->begin(), this->end(), [type](const CAudioDevice &device)
            {
                return device.getType() == type;
            });
        }

        /*
         * Register metadata
         */
        void CAudioDeviceList::registerMetadata()
        {
            qRegisterMetaType<CAudioDeviceList>();
            qDBusRegisterMetaType<CAudioDeviceList>();
            registerMetaValueType<CAudioDeviceList>();
        }

    } // namespace
} // namespace
