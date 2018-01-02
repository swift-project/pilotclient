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
#include "blackmisc/metaclass.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/network/ecosystem.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating information of a server
        class BLACKMISC_EXPORT CServer :
            public CValueObject<CServer>,
            public ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexName = CPropertyIndex::GlobalIndexCServer,
                IndexDescription,
                IndexAddress,
                IndexPort,
                IndexUser,
                IndexFsdSetup,
                IndexEcosystem,
                IndexIsAcceptingConnections,
                IndexServerType,
                IndexServerTypeAsString
            };

            //! Server Type
            enum ServerType
            {
                Unspecified,
                FSDServerVatsim,
                FSDServer,
                VoiceServerVatsim,
                VoiceServer,
                WebService,
            };

            //! Allows to iterate over all ServerType
            static const QList<int> &allServerTypes();

            //! Enum to string
            static const QString &serverTypeToString(ServerType server);

            //! Default constructor.
            CServer() {}

            //! Constructor.
            CServer(const QString &name, const QString &description, const QString &address, int port,
                    const CUser &user,
                    const CFsdSetup &setup, const CEcosystem &ecosytem, ServerType serverType,
                    bool isAcceptingConnections = true);

            //! Constructor (minimal for testing)
            CServer(const QString &address, int port, const CUser &user);

            //! Constructor by ecosystem
            CServer(const CEcosystem &ecosystem);

            //! Constructor by server type
            CServer(ServerType serverType);

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

            //! Has name?
            bool hasName() const { return !m_name.isEmpty(); }

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

            //! Get the ecosystem
            const CEcosystem &getEcosystem() const { return m_ecosystem; }

            //! Set the ecosystem
            bool setEcosystem(const CEcosystem &ecosystem);

            //! Server is accepting connections (allows to disable server temporarily or generally)
            bool isAcceptingConnections() const { return m_isAcceptingConnections; }

            //! Set whether server is accepting connections (allows to disable server temporarily or generally)
            void setIsAcceptingConnections(bool value) { m_isAcceptingConnections = value; }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Address and port?
            bool hasAddressAndPort() const;

            //! Get setup
            const CFsdSetup &getFsdSetup() const { return m_fsdSetup; }

            //! A FSD server?
            bool isFsdServer() const;

            //! Set setup
            void setFsdSetup(const CFsdSetup &setup) { m_fsdSetup = setup; }

            //! Set server type
            bool setServerType(ServerType serverType);

            //! Get server type
            ServerType getServerType() const { return static_cast<ServerType>(m_serverType); }

            //! Unspecified?
            bool hasUnspecifiedServerType() const;

            //! Get server type as string
            const QString &getServerTypeAsString() const;

            //! Connected since
            QDateTime getConnectedSince() const { return this->getUtcTimestamp(); }

            //! Mark as connected since now
            void setConnectedSinceNow() { this->setCurrentUtcTime(); }

            //! Mark as diconnected
            void markAsDisconnected() { this->setTimestampToNull(); }

            //! Is connected?
            bool isConnected() const;

            //! NULL?
            bool isNull() const;

            //! Validate, provide details about issues
            CStatusMessageList validate() const;

            //! Identifying a session, if not connected empty
            QString getServerSessionId() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const CServer &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! swift FSD test server
            static const CServer &swiftFsdTestServer(bool withPw = false);

        private:
            QString    m_name;
            QString    m_description;
            QString    m_address;
            int        m_port = -1;
            CUser      m_user;
            CFsdSetup  m_fsdSetup;
            CEcosystem m_ecosystem;
            int        m_serverType = static_cast<int>(Unspecified);
            bool       m_isAcceptingConnections = true; //!< disable server for connections

            BLACK_METACLASS(
                CServer,
                BLACK_METAMEMBER(name),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(address),
                BLACK_METAMEMBER(port),
                BLACK_METAMEMBER(user),
                BLACK_METAMEMBER(fsdSetup),
                BLACK_METAMEMBER(ecosystem),
                BLACK_METAMEMBER(serverType),
                BLACK_METAMEMBER(isAcceptingConnections),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServer)
Q_DECLARE_METATYPE(BlackMisc::Network::CServer::ServerType)

#endif // guard
