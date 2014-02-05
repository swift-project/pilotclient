/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_USER_H
#define BLACKMISC_USER_H

#include "avcallsign.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information of a user.
         */
        class CUser : public BlackMisc::CValueObject
        {
        public:

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexEmail,
                IndexId,
                IndexPassword,
                IndexRealName,
                IndexCallsign,
                IndexCallsignIcon
            };

            /*!
             * Default constructor.
             */
            CUser() {}

            /*!
             * Constructor.
             */
            CUser(const QString &id, const QString &realname, const QString &email = "", const QString &password = "")
                : m_id(id), m_realname(realname), m_email(email), m_password(password)
            {}

            /*!
             * \copydoc CValueObject::toQVariant()
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get full name.
             * \return
             */
            QString getRealName() const { return m_realname; }

            /*!
             * \brief setRealName
             */
            void setRealName(const QString &realname) { m_realname = realname.trimmed(); }

            /*!
             * Get password
             */
            QString getPassword() const { return m_password; }

            /*!
             * Set password
             */
            void setPassword(const QString &pw) { m_password = pw.trimmed(); }

            /*!
             * \brief Valid user object?
             */
            bool isValid() const { return !this->m_realname.isEmpty() &&  !this->m_id.isEmpty(); }

            /*!
             * \brief Valid credentials?
             */
            bool hasValidCredentials() const { return this->isValid() && !this->m_password.isEmpty(); }

            /*!
             * \brief Valid real name?
             */
            bool hasValidRealName() const { return !this->m_realname.isEmpty(); }

            /*!
             * \brief Valid id?
             */
            bool hasValidId() const { return !this->m_id.isEmpty(); }

            /*!
             * Has associated callsign?
             */
            bool hasValidCallsign() const { return !m_callsign.isEmpty(); }

            /*!
             * Get email.
             */
            QString getEmail() const { return m_email; }

            /*!
             * Set email.
             */
            void setEmail(const QString &email) { m_email = email.trimmed(); }

            /*!
             * \brief Valid email?
             */
            bool hasValidEmail() const { return !this->m_email.isEmpty(); }

            /*!
             * Get id.
             */
            QString getId() const { return m_id; }

            /*!
             * \brief Set id
             */
            void setId(const QString &id) { m_id = id.trimmed(); }

            /*!
             * Get associated callsign.
             */
            BlackMisc::Aviation::CCallsign getCallsign() const { return m_callsign; }

            /*!
             * \brief Set associated callsign
             * \param callsign
             */
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_callsign = callsign; }

            //! \copydoc CValueObject::toIcon()
            virtual const QPixmap &toIcon() const override
            {
                return this->getCallsign().toIcon();
            }

            /*!
             * \brief Equal operator ==
             */
            bool operator ==(const CUser &other) const;

            /*!
             * \brief Unequal operator !=
             */
            bool operator !=(const CUser &other) const;

            /*!
             * \copydoc CValueObject::getValueHash()
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * This and another user exchange missing data.
             * This user has priority and overrides first.
             * \param otherUser
             */
            void syncronizeData(CUser &otherUser);

            /*!
             * \copydoc CValueObject::propertyByIndex(int)
             */
            QVariant propertyByIndex(int index) const;

            /*!
             * \copydoc CValueObject::propertyByIndex(const QVariant, int)
             */
            void propertyByIndex(const QVariant &variant, int index);

        protected:
            /*!
             * \brief Rounded value as string
             * \param i18n
             * \return
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
            QString m_id;
            QString m_realname;
            QString m_email;
            QString m_password;
            BlackMisc::Aviation::CCallsign m_callsign;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUser)


#endif // guard
