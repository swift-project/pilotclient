/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIODEVICELIST_H
#define BLACKMISC_AUDIODEVICELIST_H

#include "audiodevice.h"
#include "sequence.h"
#include "collection.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Audio
    {
        /*!
         * Value object encapsulating a list of audio devices.
         */
        class CAudioDeviceList : public CSequence<CAudioDevice>
        {
        public:
            //! Default constructor.
            CAudioDeviceList();

            //! Construct from a base class object.
            CAudioDeviceList(const CSequence &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Get output devices in that list
            CAudioDeviceList getOutputDevices() const;

            //! Get output devices in that list
            CAudioDeviceList getInputDevices() const;

            //! Count (as of type)
            int count(CAudioDevice::DeviceType type) const;

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CAudioDevice>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Audio::CAudioDevice>)

#endif //guard
