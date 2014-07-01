/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        }

    } // namespace
} // namespace
