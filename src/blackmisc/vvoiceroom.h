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
            CVoiceRoom() :
                m_connected(false), m_audioPlaying(false) {}

            /*!
             * Constructor.
             * \param hostname
             * \param channel
             */
            CVoiceRoom(const QString &hostname, const QString &channel) :
                m_hostname(hostname), m_channel(channel), m_connected(false), m_audioPlaying(false) {}

            /*!
             * Constructor.
             * \param serverUrl
             * \param connected
             */
            CVoiceRoom(const QString &serverUrl, bool connected = false);

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get the host name
             * \return
             */
            const QString &getHostName() const { return m_hostname; }

            /*!
             * Get the voice room
             */
            const QString &getChannel() const { return m_channel; }

            /*!
             * \brief Set the host name
             */
            void setHostName(const QString &hostName) { m_hostname = hostName; }

            /*!
             * \brief Set the voice channel
             */
            void setChannel(const QString &channel) { m_channel = channel; }

            /*!
             * \brief Server URL
             * \param noProtocol either with (pseudo) protocol prefix or without
             * \return
             */
            QString getVoiceRoomUrl(bool noProtocol = true) const;

            /*!
             * \brief Valid voice room object?
             * \return
             */
            bool isValid() const { return !this->m_hostname.isEmpty() &&  !this->m_channel.isEmpty(); }

            /*!
             * \brief Is connected
             * \return
             */
            bool isConnected() const { return this->isValid() && this->m_connected; }

            /*!
             * \brief Set connected status
             * \param isConnected
             */
            void setConnected(bool isConnected) { this->m_connected = isConnected; }

            /*!
             * \brief Is audio playing in this room?
             * \return
             */
            bool isAudioPlaying() const { return this->m_audioPlaying; }

            /*!
             * \brief Set audio playing
             * \param playing
             */
            void setAudioPlaying(bool playing)  { this->m_audioPlaying = playing; }

            /*!
             * \brief Is ATIS voice channel
             * \return
             */
            bool isAtis() const;

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
             * \copydoc CValueObject::getValueHash
             */
            virtual uint getValueHash() const override;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Protocol prefix
             * \return
             */
            static const QString &protocol() { static QString p("vvl"); return p; }

            /*!
             * \brief Protocol
             * \return with protocol prefix or without
             */
            static const QString &protocolComplete() { static QString p("vvl://"); return p; }

        protected:

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            QString m_hostname;
            QString m_channel;
            bool m_connected;
            bool m_audioPlaying;
        };
    } // Voice
} // BlackMisc

Q_DECLARE_METATYPE(BlackMisc::Voice::CVoiceRoom)

#endif // guard
