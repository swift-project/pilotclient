/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_SERVER_H
#define BLACKMISC_SERVER_H
#include "nwuser.h"
#include "valueobject.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information of a server
         */
        class CServer : public BlackMisc::CValueObject
        {
        public:
            /*!
             * Default constructor.
             */
            CServer() : m_port(-1) {}

            /*!
             * Constructor.
             * \param name
             * \param description
             * \param address
             * \param port
             * \param user
             */
            CServer(const QString &name, const QString &description, const QString &address, qint32 port, const CUser &user)
                : m_name(name), m_description(description), m_address(address), m_port(port), m_user(user) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get address.
             * \return
             */
            const QString &getAddress() const { return m_address; }

            /*!
             * \brief Set address
             * \param realname
             */
            void setAddress(const QString &address) { m_address = address; }

            /*!
             * Get user
             * \return
             */
            const CUser &getUser() const { return m_user; }

            /*!
             * \brief Set user
             * \param password
             */
            void setUser(const CUser &user) { m_user = user; }

            /*!
             * Get name
             * \return
             */
            const QString &getName() const { return m_name; }

            /*!
             * \brief Set name
             * \param name
             */
            void setName(const QString &name) { m_name = name; }

            /*!
             * Get description
             * \return
             */
            const QString &getDescription() const { return m_description; }

            /*!
             * \brief Set description
             * \param description
             */
            void setDescription(const QString &description) { m_description = description; }

            /*!
             * Get port
             * \return
             */
            qint32 getPort() const { return m_port; }

            /*!
             * \brief Set port
             * \param port
             */
            void setPort(qint32 port) { m_port = port; }

            /*!
             * \brief Is valid for login
             * \return
             */
            bool isValidForLogin() const;

            /*!
             * \brief Equal operator ==
             * \param other
             * \return
             */
            bool operator ==(const CServer &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * \return
             */
            bool operator !=(const CServer &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexName = 0,
                IndexDescription,
                IndexAddress,
                IndexPort,
                IndexUserId,
                IndexUserRealName,
                IndexUserPassword
            };

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index as string
             * \param index
             * \param i18n
             * \return
             */
            QString propertyByIndexAsString(int index, bool i18n) const;

            /*!
             * \brief Property by index (setter)
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
            QString m_name;
            QString m_description;
            QString m_address;
            qint32 m_port;
            CUser m_user;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServer)

#endif // guard
