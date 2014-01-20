/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_TEXTMESSAGE_H
#define BLACKMISC_TEXTMESSAGE_H

#include "pqfrequency.h"
#include "avcallsign.h"
#include "valueobject.h"
#include <QDateTime>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information of a textserver
         */
        class CTextMessage : public BlackMisc::CValueObject
        {
        public:
            /*!
             * Default constructor.
             */
            CTextMessage() : m_received(QDateTime::currentDateTimeUtc()), m_frequency(-1.0, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()) {}

            /*!
             * Constructor, radio message
             */
            CTextMessage(const QString &message, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Aviation::CCallsign &senderCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_received(QDateTime::currentDateTimeUtc()), m_senderCallsign(senderCallsign), m_frequency(frequency)
            {
                this->m_frequency.switchUnit(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            /*!
             * Constructor, private message
             */
            CTextMessage(const QString &message, const BlackMisc::Aviation::CCallsign &senderCallsign, const BlackMisc::Aviation::CCallsign &recipientCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_received(QDateTime::currentDateTimeUtc()), m_senderCallsign(senderCallsign), m_recipientCallsign(recipientCallsign), m_frequency(-1.0, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get callsign (from)
             */
            const BlackMisc::Aviation::CCallsign &getSenderCallsign() const
            {
                return m_senderCallsign;
            }

            /*!
             * Set callsign (from)
             */
            void setSenderCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                m_senderCallsign = callsign;
            }

            /*!
             * Get callsign (to)
             */
            const BlackMisc::Aviation::CCallsign &getRecipientCallsign() const
            {
                return m_recipientCallsign;
            }

            /*!
             * Set callsign (to)
             */
            void setRecipientCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                m_recipientCallsign = callsign;
            }

            /*!
             * Send to particular frequency?
             */
            bool isSendToFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const;

            /*!
             * Send to UNICOM?
             */
            bool isSendToUnicom() const;

            /*!
             * \brief Valid receviver?
             */
            bool hasValidRecipient() const;

            /*!
             * Get message
             */
            const QString &getMessage() const
            {
                return m_message;
            }

            /*!
             * Empty message
             */
            bool isEmpty() const
            {
                return m_message.isEmpty();
            }

            /*!
             * Set message
             */
            void setMessage(const QString &message)
            {
                m_message = message.trimmed();
            }

            /*!
             * Get frequency
             */
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const
            {
                return m_frequency;
            }

            /*!
             * Set frequency
             */
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                m_frequency = frequency;
            }

            /*!
             * \brief Is private message?
             */
            bool isPrivateMessage() const;

            /*!
             * \brief Is radio message?
             */
            bool isRadioMessage() const;

            /*!
             * \brief Initial message of server?
             */
            bool isServerMessage() const;

            /*!
             * \brief Received (hh mm ss)
             */
            QString receivedTime() const
            {
                QString rt = this->m_received.toString("hh::mm::ss");
                return rt;
            }

            /*!
             * Whole message as formatted string.
             * Used to display message in a console window.
             * \param withSender        include sender information in string?
             * \param withRecipient     include recipient information in string?
             * \param separator         values separated by given value
             * \return
             */
            QString asString(bool withSender, bool withRecipient, const QString separator = ", ") const;

            /*!
             * Toggle sender receiver, can be used to ping my own message
             */
            void toggleSenderRecipient();

            /*!
             * \brief Equal operator ==
             */
            bool operator ==(const CTextMessage &other) const;

            /*!
             * \brief Unequal operator !=
             */
            bool operator !=(const CTextMessage &other) const;

            /*!
             * \copydoc CValueObject::getValueHash()
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        protected:
            /*!
             * \copydoc CValueObject::convertToQString
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \copydoc CValueObject::getMetaTypeId
             */
            virtual int getMetaTypeId() const;

            /*!
             * \copydoc CValueObject::isA
             */
            virtual bool isA(int metaTypeId) const;

            /*!
             * \copydoc CValueObject::compareImpl
             */
            virtual int compareImpl(const CValueObject &other) const;

            /*!
             * \copydoc CValueObject::marshallToDbus
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \copydoc CValueObject::unmarshallFomDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

        private:
            QString m_message;
            QDateTime m_received;
            BlackMisc::Aviation::CCallsign m_senderCallsign;
            BlackMisc::Aviation::CCallsign m_recipientCallsign;
            BlackMisc::PhysicalQuantities::CFrequency m_frequency;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CTextMessage)

#endif // guard
