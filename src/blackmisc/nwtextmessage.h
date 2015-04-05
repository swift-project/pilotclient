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

#include "blackmisc/timestampbased.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/aviation/callsign.h"
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
        class CTextMessage :
            public CValueObject<CTextMessage>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexSenderCallsign = BlackMisc::CPropertyIndex::GlobalIndexCTextMessage,
                IndexRecipientCallsign,
                IndexRecipientCallsignOrFrequency,
                IndexMessage
            };

            //! Default constructor.
            CTextMessage() {}

            //! Constructor, radio message
            CTextMessage(const QString &message, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Aviation::CCallsign &senderCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_senderCallsign(senderCallsign), m_frequency(frequency)
            {
                this->m_frequency.switchUnit(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! Constructor, private message
            CTextMessage(const QString &message, const BlackMisc::Aviation::CCallsign &senderCallsign, const BlackMisc::Aviation::CCallsign &recipientCallsign = BlackMisc::Aviation::CCallsign())
                : m_message(message), m_senderCallsign(senderCallsign), m_recipientCallsign(recipientCallsign), m_frequency(0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit()) {}

            //! Get callsign (from)
            const BlackMisc::Aviation::CCallsign &getSenderCallsign() const { return m_senderCallsign; }

            //! Set callsign (from)
            void setSenderCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_senderCallsign = callsign;}

            //! Get callsign (to)
            const BlackMisc::Aviation::CCallsign &getRecipientCallsign() const { return m_recipientCallsign; }

            //! Set callsign (recipient)
            void setRecipientCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_recipientCallsign = callsign; }

            //! Get recipient or frequency
            QString getRecipientCallsignOrFrequency() const;

            //! Send to particular frequency?
            bool isSendToFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const;

            //! Send to UNICOM?
            bool isSendToUnicom() const;

            //! Valid receviver?
            bool hasValidRecipient() const;

            //! Get message
            const QString &getMessage() const { return m_message; }

            //! Empty message
            bool isEmpty() const { return m_message.isEmpty(); }

            //! Set message
            void setMessage(const QString &message) { m_message = message.trimmed(); }

            //! Get frequency
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const { return m_frequency; }

            //! Set frequency
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) {  m_frequency = frequency; }

            //! Is private message?
            bool isPrivateMessage() const;

            //! Is radio message?
            bool isRadioMessage() const;

            //! Initial message of server?
            bool isServerMessage() const;

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

            //! Toggle sender receiver, can be used to ping my own message
            void toggleSenderRecipient();

            /*!
             * Is this a text message encapsulating a SELCAL
             * \see http://forums.vatsim.net/viewtopic.php?f=8&t=63467
             */
            bool isSelcalMessage() const;

            //! Is this a text message encapsulating a SELCAL for given code?
            bool isSelcalMessageFor(const QString &selcal) const;

            //! Supervisor message?
            bool isSupervisorMessage() const;

            //! Was sent?
            bool wasSent() const;

            //! Mark as sent
            void markAsSent();

            //! Get SELCAL code (if applicable, e.g. ABCD), otherwise ""
            QString getSelcalCode() const;

            //! As icon, not implement by all classes
            virtual CIcon toIcon() const override;

            //! As pixmap, required for most GUI views
            virtual QPixmap toPixmap() const override;

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTextMessage)
            QString m_message;
            BlackMisc::Aviation::CCallsign m_senderCallsign;
            BlackMisc::Aviation::CCallsign m_recipientCallsign;
            BlackMisc::PhysicalQuantities::CFrequency m_frequency {0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit()};
            bool m_wasSent = false;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CTextMessage, (o.m_message, o.m_timestampMSecsSinceEpoch, o.m_senderCallsign, o.m_recipientCallsign, o.m_frequency))
Q_DECLARE_METATYPE(BlackMisc::Network::CTextMessage)

#endif // guard
