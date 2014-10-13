/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEXTMESSAGE_H
#define BLACKMISC_TEXTMESSAGE_H

#include "pqfrequency.h"
#include "avcallsign.h"
#include "statusmessage.h"
#include "valueobject.h"
#include <QDateTime>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information of a text message
         */
        class CTextMessage : public BlackMisc::CValueObject
        {
        public:
            //! \brief Default constructor.
            CTextMessage() : m_received(QDateTime::currentDateTimeUtc()), m_frequency(0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit()) {}

            //! \brief Constructor, radio message
            CTextMessage(const QString &message, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Aviation::CCallsign &senderCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_received(QDateTime::currentDateTimeUtc()), m_senderCallsign(senderCallsign), m_frequency(frequency)
            {
                this->m_frequency.switchUnit(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! \brief Constructor, private message
            CTextMessage(const QString &message, const BlackMisc::Aviation::CCallsign &senderCallsign, const BlackMisc::Aviation::CCallsign &recipientCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_received(QDateTime::currentDateTimeUtc()), m_senderCallsign(senderCallsign), m_recipientCallsign(recipientCallsign), m_frequency(0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit()) {}

            //! \brief Get callsign (from)
            const BlackMisc::Aviation::CCallsign &getSenderCallsign() const
            {
                return m_senderCallsign;
            }

            //! \brief Set callsign (from)
            void setSenderCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                m_senderCallsign = callsign;
            }

            //! \brief Get callsign (to)
            const BlackMisc::Aviation::CCallsign &getRecipientCallsign() const
            {
                return m_recipientCallsign;
            }

            //! \brief Set callsign (recipient)
            void setRecipientCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                m_recipientCallsign = callsign;
            }

            //! \brief Send to particular frequency?
            bool isSendToFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const;

            //! \brief Send to UNICOM?
            bool isSendToUnicom() const;

            //! \brief Valid receviver?
            bool hasValidRecipient() const;

            //! \brief Get message
            const QString &getMessage() const
            {
                return m_message;
            }

            //! \brief Empty message
            bool isEmpty() const
            {
                return m_message.isEmpty();
            }

            //! \brief Set message
            void setMessage(const QString &message)
            {
                m_message = message.trimmed();
            }

            //! \brief Get frequency
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const
            {
                return m_frequency;
            }

            //! \brief Set frequency
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                m_frequency = frequency;
            }

            //! \brief Is private message?
            bool isPrivateMessage() const;

            //! \brief Is radio message?
            bool isRadioMessage() const;

            //! \brief Initial message of server?
            bool isServerMessage() const;

            //! \brief Received (hh mm ss)
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
            QString asString(bool withSender, bool withRecipient, const QString &separator = ", ") const;

            /*!
             * Whole message as BlackMisc::CStatusMessage.
             * Used to display message in logs, or any other situation where a CStatusMessage is required.
             * \param withSender        include sender information in string?
             * \param withRecipient     include recipient information in string?
             * \param separator         values separated by given value
             * \return
             */
            BlackMisc::CStatusMessage asStatusMessage(bool withSender, bool withRecipient, const QString &separator = ", ") const;

            //! \brief Toggle sender receiver, can be used to ping my own message
            void toggleSenderRecipient();

            /*!
             * \brief Is this a text message encapsulating a SELCAL
             * \see http://forums.vatsim.net/viewtopic.php?f=8&t=63467
             */
            bool isSelcalMessage() const;

            /*!
             * \brief Is this a text message encapsulating a SELCAL for given code
             */
            bool isSelcalMessageFor(const QString &selcal) const;

            //! \brief Get SELCAL code (if applicable, e.g. ABCD), otherwise ""
            QString getSelcalCode() const;

            //! \brief Equal operator ==
            bool operator ==(const CTextMessage &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CTextMessage &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \brief Register metadata
            static void registerMetadata();

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

            //! \copydoc CValueObject::unmarshallFomDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTextMessage)
            QString m_message;
            QDateTime m_received;
            BlackMisc::Aviation::CCallsign m_senderCallsign;
            BlackMisc::Aviation::CCallsign m_recipientCallsign;
            BlackMisc::PhysicalQuantities::CFrequency m_frequency;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CTextMessage, (o.m_message, o.m_received, o.m_senderCallsign, o.m_recipientCallsign, o.m_frequency))
Q_DECLARE_METATYPE(BlackMisc::Network::CTextMessage)

#endif // guard
