/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "audiodeviceinfolist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Audio
    {
        /*
         * Default constructor
         */
        CAudioDeviceInfoList::CAudioDeviceInfoList() { }

        /*
         * Construct from base class object
         */
        CAudioDeviceInfoList::CAudioDeviceInfoList(const CSequence &other) :
            CSequence(other)
        { }

        /*
         * Output devices
         */
        CAudioDeviceInfoList CAudioDeviceInfoList::getOutputDevices() const
        {
            return this->findBy(&CAudioDeviceInfo::getType, CAudioDeviceInfo::OutputDevice);
        }

        /*
         * Output devices
         */
        CAudioDeviceInfoList CAudioDeviceInfoList::getInputDevices() const
        {
            return this->findBy(&CAudioDeviceInfo::getType, CAudioDeviceInfo::InputDevice);
        }

        /*
         * Count as of type
         */
        int CAudioDeviceInfoList::count(CAudioDeviceInfo::DeviceType type) const
        {
            return std::count_if(this->begin(), this->end(), [type](const CAudioDeviceInfo &device)
            {
                return device.getType() == type;
            });
        }

        /*
         * Register metadata
         */
        void CAudioDeviceInfoList::registerMetadata()
        {
            qRegisterMetaType<CAudioDeviceInfoList>();
            qDBusRegisterMetaType<CAudioDeviceInfoList>();
            registerMetaValueType<CAudioDeviceInfoList>();
        }

    } // namespace
} // namespace
