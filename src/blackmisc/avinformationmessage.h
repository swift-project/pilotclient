/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_INFORMATIONMESSAGE_H
#define BLACKMISC_INFORMATIONMESSAGE_H

#include "valueobject.h"
#include <QString>
#include <QDateTime>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information message.
         * ATIS, METAR, TAF
         */
        class CInformationMessage : public BlackMisc::CValueObject
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

            //! \brief Default constructor.
            CInformationMessage() : m_type(CInformationMessage::Unspecified), m_receivedTimestamp(QDateTime::currentDateTimeUtc())
            {}

            //! \brief Information message of type
            explicit CInformationMessage(InformationType type) : m_type(type) {}

            /*!
             * \brief Information message of type
             * \param type
             * \param message
             */
            CInformationMessage(InformationType type, const QString &message)
                : m_type(type), m_message(message)
            {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Equal operator ==
            bool operator ==(const CInformationMessage &other) const;

            //! \brief operator !=
            bool operator !=(const CInformationMessage &other) const;

            //! \brief Get message.
            const QString &getMessage() const { return m_message; }

            //! \brief Is a message available
            bool hasMessage() const { return !m_message.isEmpty(); }

            //! \brief Set message
            void setMessage(const QString &message)
            {
                this->m_receivedTimestamp = QDateTime::currentDateTimeUtc();
                this->m_message = message;
            }

            //! \brief Append message part
            void appendMessage(const QString &messagePart)
            {
                this->m_receivedTimestamp = QDateTime::currentDateTimeUtc();
                this->m_message.append(messagePart);
            }

            //! \brief Type as string
            const QString &getTypeAsString() const;

            //! \brief Type
            InformationType getType() const { return this->m_type; }

            //! \brief Set type
            void setType(InformationType type) { this->m_type = type; }

            //! \brief Timestamp
            const QDateTime &getReceivedTimestamp() const { return this->m_receivedTimestamp; }

            //! \brief Received before n ms
            qint64 timeDiffReceivedMs() const
            {
                return this->m_receivedTimestamp.msecsTo(QDateTime::currentDateTimeUtc());
            }

            //! \brief Is empty?
            bool isEmpty() const { return this->m_message.isEmpty(); }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \brief Register metadata
            static void registerMetadata();

            //! \brief Members
            static const QStringList &jsonMembers();

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
            BLACK_ENABLE_TUPLE_CONVERSION(CInformationMessage)
            InformationType m_type;
            QString m_message;
            QDateTime m_receivedTimestamp;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CInformationMessage, (o.m_type, o.m_message, o.m_receivedTimestamp))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CInformationMessage)

#endif // guard
