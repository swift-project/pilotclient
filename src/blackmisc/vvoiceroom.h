/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "valueobject.h"

#include <QString>


#ifndef BLACKMISC_VOICEROOM_H
#define BLACKMISC_VOICEROOM_H

namespace BlackMisc
{
    namespace Voice
    {
        /*!
         * Value object encapsulating information of a voice room
         */
        class CVoiceRoom : public BlackMisc::CValueObject
        {
        public:

            /*!
             * Default constructor.
             */
            CVoiceRoom() : m_hostname(""), m_channel("") {}

            /*!
             * Constructor.
             */
            CVoiceRoom(const QString &hostname, const QString &channel) : m_hostname(hostname), m_channel(channel) {}

            /*!
             * Constructor.
             */
            CVoiceRoom(const QString &serverSpec);

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get the host name
             * \return
             */
            const QString &hostName() const { return m_hostname; }

            /*!
             * Get the voice room
             * \return
             */
            const QString &channel() const { return m_channel; }

            /*!
             * Set the host name
             * \param
             */
            void setHostName(const QString &hostName) { m_hostname = hostName; }

            /*!
             * Set the voice channel
             * \param
             */
            void setChannel(const QString &channel) { m_channel = channel; }

            /*!
             * \brief Valid voice room object?
             * \return
             */
            bool isValid() const { return !this->m_hostname.isEmpty() &&  !this->m_channel.isEmpty(); }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CVoiceRoom &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CVoiceRoom &other) const;

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


        private:
            QString m_hostname;
            QString m_channel;

        };
    } // Voice
} // BlackMisc

Q_DECLARE_METATYPE(BlackMisc::Voice::CVoiceRoom)

#endif // BLACKMISC_VOICEROOM_H
