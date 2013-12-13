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

            /*!
             * Default constructor.
             */
            CInformationMessage(CInformationMessage::InformationType type = CInformationMessage::Unspecified)
                : m_type(type), m_receivedTimestamp(QDateTime::currentDateTimeUtc())
            {}

            /*!
             * \brief Information message of type
             * \param type
             * \param message
             */
            CInformationMessage(InformationType type, const QString &message)
                : m_type(type), m_message(message)
            {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CInformationMessage &other) const;

            /*!
             * \brief operator !=
             * \param other
             * \return
             */
            bool operator !=(const CInformationMessage &other) const;

            /*!
             * Get message.
             * \return
             */
            const QString &getMessage() const { return m_message; }

            /*!
             * \brief Is a message available
             * \return
             */
            bool hasMessage() const { return !m_message.isEmpty(); }

            /*!
             * \brief Add message
             * \remarks ATIS message come in as parts, this here features some logic, to either append or reset the message
             * \param message
             */
            void addMessage(const QString &message);

            /*!
             * \brief Set message
             * \param message
             */
            void setMessage(const QString &message) { m_message = message; }

            /*!
             * \brief Type as string
             * \return
             */
            const QString &getTypeAsString() const;

            /*!
             * \brief Type
             * \return
             */
            InformationType getType() const { return this->m_type; }

            /*!
             * \brief Timestamp
             * \return
             */
            const QDateTime &getReceivedTimestamp() const { return this->m_receivedTimestamp; }

            /*!
             * \brief Received before n ms
             * \return
             */
            qint64 timeDiffReceivedMs() const
            {
                return this->m_receivedTimestamp.msecsTo(QDateTime::currentDateTimeUtc());
            }

            /*!
             * \brief Is empty
             * \return
             */
            bool isEmpty() const { return this->m_message.isEmpty(); }

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
            InformationType m_type;
            QString m_message;
            QDateTime m_receivedTimestamp;

            /*!
             * \brief Outdated timestamp?
             * \return
             */
            bool isOutdated() const;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CInformationMessage)

#endif // guard
