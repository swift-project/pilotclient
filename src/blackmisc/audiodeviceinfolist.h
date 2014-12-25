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

#include "audiodeviceinfo.h"
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
        class CAudioDeviceInfoList : public CSequence<CAudioDeviceInfo>
        {
        public:
            //! Default constructor.
            CAudioDeviceInfoList();

            //! Construct from a base class object.
            CAudioDeviceInfoList(const CSequence &other);

            //! Get output devices in that list
            CAudioDeviceInfoList getOutputDevices() const;

            //! Get output devices in that list
            CAudioDeviceInfoList getInputDevices() const;

            //! Count (as of type)
            int count(CAudioDeviceInfo::DeviceType type) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CAudioDeviceInfo>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Audio::CAudioDeviceInfo>)

#endif //guard
