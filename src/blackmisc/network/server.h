/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_SERVER_H
#define BLACKMISC_NETWORK_SERVER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/user.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating information of a server
        class BLACKMISC_EXPORT CServer : public CValueObject<CServer>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexName = BlackMisc::CPropertyIndex::GlobalIndexCServer,
                IndexDescription,
                IndexAddress,
                IndexPort,
                IndexUser,
                IndexIsAcceptingConnections
            };

            //! Default constructor.
            CServer() {}

            //! Constructor.
            CServer(const QString &name, const QString &description, const QString &address, int port, const CUser &user, bool isAcceptingConnections = true)
                : m_name(name), m_description(description), m_address(address), m_port(port), m_user(user), m_isAcceptingConnections(isAcceptingConnections) {}

            //! Get address.
            const QString &getAddress() const { return m_address; }

            //! Set address (e.g. myserver.foo.com)
            void setAddress(const QString &address) { m_address = address.trimmed(); }

            //! Get user
            const CUser &getUser() const { return m_user; }

            //! Set user
            void setUser(const CUser &user) { m_user = user; }

            //! Get name
            const QString &getName() const { return m_name; }

            //! Set name
            void setName(const QString &name) { m_name = name.trimmed(); }

            //! Matches server name?
            bool matchesName(const QString &name) const;

            //! Same address and port?
            bool matchesAddressPort(const CServer &server) const;

            //! Same address?
            bool matchesAddress(const QString &address) const;

            //! Get description
            const QString &getDescription() const { return m_description; }

            //! Set description
            void setDescription(const QString &description) { m_description = description.trimmed().simplified(); }

            //! Get port
            int getPort() const { return m_port; }

            //! Set port
            void setPort(int port) { m_port = port; }

            //! Server is accepting connections (allows to disable server temporarily)
            bool isAcceptingConnections() const { return m_isAcceptingConnections; }

            //! Set whether server is accepting connections (allows to disable server temporarily)
            void setIsAcceptingConnections(bool value) { m_isAcceptingConnections = value; }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Address and port?
            bool hasAddressAndPort() const;

            //! Validate, provide details about issues
            BlackMisc::CStatusMessageList validate() const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString()
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CServer)
            QString m_name;
            QString m_description;
            QString m_address;
            int     m_port = -1;
            CUser   m_user;
            bool    m_isAcceptingConnections = true; //!< temp. disable server
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CServer, (o.m_name, o.m_description, o.m_address, o.m_port, o.m_user, o.m_isAcceptingConnections))
Q_DECLARE_METATYPE(BlackMisc::Network::CServer)

#endif // guard
