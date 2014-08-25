/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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

            //! Properties by index
            enum ColumnIndex
            {
                IndexName = 0,
                IndexDescription,
                IndexAddress,
                IndexPort,
                IndexUser,
                IndexIsAcceptingConnections
            };

            //! Default constructor.
            CServer() : m_port(-1), m_isAcceptingConnections(true) {}

            //! Constructor.
            CServer(const QString &name, const QString &description, const QString &address, qint32 port, const CUser &user, bool isAcceptingConnections = true)
                : m_name(name), m_description(description), m_address(address), m_port(port), m_user(user), m_isAcceptingConnections(isAcceptingConnections) {}

            //! Get address.
            const QString &getAddress() const { return m_address; }

            //! Set address (e.g. myserver.foo.com)
            void setAddress(const QString &address) { m_address = address; }

            //! Get user
            const CUser &getUser() const { return m_user; }

            //! Set user
            void setUser(const CUser &user) { m_user = user; }

            //! Get name
            const QString &getName() const { return m_name; }

            //! Set name
            void setName(const QString &name) { m_name = name; }

            //! Get description
            const QString &getDescription() const { return m_description; }

            //! Set description
            void setDescription(const QString &description) { m_description = description; }

            //! Get port
            qint32 getPort() const { return m_port; }

            //! Set port
            void setPort(qint32 port) { m_port = port; }

            //! Server is accepting connections
            bool isAcceptingConnections() const { return m_isAcceptingConnections; }

            //! Set whether server is accepting connections
            void setIsAcceptingConnections(bool value) { m_isAcceptingConnections = value; }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Equal operator ==
            bool operator ==(const CServer &other) const;

            //! Unequal operator !=
            bool operator !=(const CServer &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::propertyByIndex(int)
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(const QVariant &, int index)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! JSON member names
            static const QStringList &jsonMembers();

            //! Register metadata
            static void registerMetadata();

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
            BLACK_ENABLE_TUPLE_CONVERSION(CServer)
            QString m_name;
            QString m_description;
            QString m_address;
            qint32 m_port;
            CUser m_user;
            bool m_isAcceptingConnections;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CServer, (o.m_name, o.m_description, o.m_address, o.m_port, o.m_user, o.m_isAcceptingConnections))
Q_DECLARE_METATYPE(BlackMisc::Network::CServer)

#endif // guard
