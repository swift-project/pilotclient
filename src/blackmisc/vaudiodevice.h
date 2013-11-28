/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "valueobject.h"

#include <QString>

#ifndef BLACKMISC_AUDIODEVICE_H
#define BLACKMISC_AUDIODEVICE_H

namespace BlackMisc
{
    namespace Voice
    {
        /*!
         * \brief Value object encapsulating information of a audio device.
         * If you want to safe this object, use the name instead of the index, since the index can change after
         * a restart.
         */
        class CAudioDevice : public BlackMisc::CValueObject
        {
        public:

            /*!
             * Default constructor.
             */
            CAudioDevice() : m_deviceIndex(-1), m_deviceName("") {}


            /*!
             * Constructor.
             */
            CAudioDevice(const int16_t index, const QString &name) : m_deviceIndex(index), m_deviceName(name) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get the device index
             * \return
             */
            int16_t index() const { return m_deviceIndex; }

            /*!
             * Get the device name
             * \return
             */
            const QString &name() const { return m_deviceName; }

            /*!
             * \brief Valid voice room object?
             * \return
             */
            bool isValid() const { return m_deviceIndex >= 0 && !m_deviceName.isEmpty(); }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CAudioDevice &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CAudioDevice &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        protected:

            /*!
             * \brief Rounded value as string
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

        protected:
            /*!
             * deviceIndex is the number is the reference for the VVL. The device is selected by this index.
             * The managing class needs to take care, that indexes are valid.
             */
            int16_t m_deviceIndex;
            QString m_deviceName;

        };

        class CInputAudioDevice : public CAudioDevice
        {
        public:
            /*!
             * Default constructor.
             */
            CInputAudioDevice() : CAudioDevice() {}


            /*!
             * Constructor.
             */
            CInputAudioDevice(const int16_t index, const QString &name) : CAudioDevice(index, name) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };

        class COutputAudioDevice : public CAudioDevice
        {
        public:
            /*!
             * Default constructor.
             */
            COutputAudioDevice() : CAudioDevice() {}


            /*!
             * Constructor.
             */
            COutputAudioDevice(const int16_t index, const QString &name) : CAudioDevice(index, name) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };
    } // Voice
} // BlackMisc

Q_DECLARE_METATYPE(BlackMisc::Voice::CAudioDevice)
Q_DECLARE_METATYPE(BlackMisc::Voice::COutputAudioDevice)
Q_DECLARE_METATYPE(BlackMisc::Voice::CInputAudioDevice)


#endif // BLACKMISC_AUDIODEVICE_H
