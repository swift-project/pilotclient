/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_USER_H
#define BLACKMISC_USER_H
#include "valueobject.h"

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
                IndexRealName
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
             * \brief QVariant, required for DBus QVariant lists
             * \return
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
             * \param realname
             */
            void setRealName(const QString &realname) { m_realname = realname.trimmed(); }

            /*!
             * Get password
             * \return
             */
            QString getPassword() const { return m_password; }

            /*!
             * Set password
             * \param
             */
            void setPassword(const QString &pw) { m_password = pw.trimmed(); }

            /*!
             * \brief Valid user object?
             * \return
             */
            bool isValid() const { return !this->m_realname.isEmpty() &&  !this->m_id.isEmpty(); }

            /*!
             * \brief Valid credentials?
             * \return
             */
            bool hasValidCredentials() const { return this->isValid() && !this->m_password.isEmpty(); }

            /*!
             * \brief Valid real name?
             * \return
             */
            bool hasValidRealName() const { return !this->m_realname.isEmpty(); }

            /*!
             * \brief Valid id?
             * \return
             */
            bool hasValidId() const { return !this->m_id.isEmpty(); }

            /*!
             * Get email.
             * \return
             */
            QString getEmail() const { return m_email; }

            /*!
             * Set email.
             * \param
             */
            void setEmail(const QString &email) { m_email = email.trimmed(); }

            /*!
             * Get id.
             * \return
             */
            QString getId() const { return m_id; }

            /*!
             * \brief Valid email?
             * \return
             */
            bool hasValidEmail() const { return !this->m_email.isEmpty(); }

            /*!
             * \brief Set id
             * \param realname
             */
            void setId(const QString &id) { m_id = id.trimmed(); }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CUser &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CUser &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * This another user exchange missing data
             * This user has priority and overrides first
             * \param otherUser
             */
            void syncronizeData(CUser &otherUser);

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index
             * \param variant
             * \param index
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
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUser)


#endif // guard
