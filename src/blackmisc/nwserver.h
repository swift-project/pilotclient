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
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get address.
             * \return
             */
            const QString &getAddress() const { return m_address; }

            /*!
             * \brief Set address (e.g. myserver.foo.com)
             */
            void setAddress(const QString &address) { m_address = address; }

            /*!
             * Get user
             */
            const CUser &getUser() const { return m_user; }

            /*!
             * \brief Set user
             */
            void setUser(const CUser &user) { m_user = user; }

            /*!
             * Get name
             */
            const QString &getName() const { return m_name; }

            /*!
             * \brief Set name
             * \param name
             */
            void setName(const QString &name) { m_name = name; }

            /*!
             * Get description
             */
            const QString &getDescription() const { return m_description; }

            /*!
             * \brief Set description
             */
            void setDescription(const QString &description) { m_description = description; }

            /*!
             * Get port
             */
            qint32 getPort() const { return m_port; }

            /*!
             * \brief Set port
             */
            void setPort(qint32 port) { m_port = port; }

            /*!
             * \brief Is valid for login?
             */
            bool isValidForLogin() const;

            /*!
             * \brief Equal operator ==
             */
            bool operator ==(const CServer &other) const;

            /*!
             * \brief Unequal operator !=
             */
            bool operator !=(const CServer &other) const;

            /*!
             * \copydoc CValueObject::getValueHash()
             */
            virtual uint getValueHash() const override;

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
             * \copydoc CValueObject::propertyByIndex(int)
             */
            QVariant propertyByIndex(int index) const override;

            /*!
             * \copydoc CValueObject::setPropertyByIndex(const QVariant &, int index)
             */
            void setPropertyByIndex(const QVariant &variant, int index) override;

            /*!
             * \copydoc CValueObject::propertyByIndexAsString()
             */
            QString propertyByIndexAsString(int index, bool i18n) const;

        protected:
            //! \copydoc CValueObject::convertToQString()
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
