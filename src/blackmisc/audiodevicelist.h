/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

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
            /*!
             * \brief Default constructor.
             */
            CAudioDeviceList();

            /*!
             * \brief Construct from a base class object.
             * \param other
             */
            CAudioDeviceList(const CSequence &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            /*!
             * \brief Get output devices in that list
             * \return
             */
            CAudioDeviceList getOutputDevices() const;

            /*!
             * \brief Get output devices in that list
             * \return
             */
            CAudioDeviceList getInputDevices() const;

            /*!
             * \brief Count (as of type)
             * \param type
             * \return
             */
            int count(CAudioDevice::DeviceType type) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CAudioDevice>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Audio::CAudioDevice>)

#endif //guard
