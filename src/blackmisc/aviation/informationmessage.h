/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_INFORMATIONMESSAGE_H
#define BLACKMISC_AVIATION_INFORMATIONMESSAGE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QString>
#include <QDateTime>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information message (ATIS, METAR, TAF)
        class BLACKMISC_EXPORT CInformationMessage : public CValueObject<CInformationMessage>
        {
        public:
            /*!
             * Type
             */
            enum InformationType
            {
                Unspecified,
                ATIS,
                METAR,
                TAF
            };

            //! Default constructor.
            CInformationMessage() : m_type(CInformationMessage::Unspecified), m_receivedTimestamp(QDateTime::currentDateTimeUtc())
            {}

            //! Information message of type
            explicit CInformationMessage(InformationType type) : m_type(type) {}

            //! Information message of type
            CInformationMessage(InformationType type, const QString &message)
                : m_type(type), m_message(message)
            {}

            //! Get message.
            const QString &getMessage() const { return m_message; }

            //! Is a message available
            bool hasMessage() const { return !m_message.isEmpty(); }

            //! Set message
            void setMessage(const QString &message)
            {
                this->m_receivedTimestamp = QDateTime::currentDateTimeUtc();
                this->m_message = message;
            }

            //! Append message part
            void appendMessage(const QString &messagePart)
            {
                this->m_receivedTimestamp = QDateTime::currentDateTimeUtc();
                this->m_message.append(messagePart);
            }

            //! Type as string
            const QString &getTypeAsString() const;

            //! Type
            InformationType getType() const { return this->m_type; }

            //! Set type
            void setType(InformationType type) { this->m_type = type; }

            //! Timestamp
            const QDateTime &getReceivedTimestamp() const { return this->m_receivedTimestamp; }

            //! Received before n ms
            qint64 timeDiffReceivedMs() const
            {
                return this->m_receivedTimestamp.msecsTo(QDateTime::currentDateTimeUtc());
            }

            //! Is empty?
            bool isEmpty() const { return this->m_message.isEmpty(); }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CInformationMessage)
            InformationType m_type;
            QString m_message;
            QDateTime m_receivedTimestamp;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CInformationMessage, (
                                   o.m_type,
                                   o.m_message,
                                   o.m_receivedTimestamp
                               ))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CInformationMessage)

#endif // guard
