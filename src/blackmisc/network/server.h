// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_SERVER_H
#define BLACKMISC_NETWORK_SERVER_H

#include "blackmisc/network/user.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/network/ecosystem.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CServer)

namespace BlackMisc::Network
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
            IndexName = CPropertyIndexRef::GlobalIndexCServer,
            IndexDescription,
            IndexAddress,
            IndexPort,
            IndexUser,
            IndexFsdSetup,
            IndexVoiceSetup,
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
                const CFsdSetup &fsdSetup, const Audio::CVoiceSetup &voiceSetup,
                const CEcosystem &ecosytem, ServerType serverType,
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
        void setAddress(const QString &address);

        //! Get user
        const CUser &getUser() const { return m_user; }

        //! Set user
        void setUser(const CUser &user) { m_user = user; }

        //! Get name
        const QString &getName() const { return m_name; }

        //! Has name?
        bool hasName() const { return !m_name.isEmpty(); }

        //! Set name
        void setName(const QString &name);

        //! Matches server name?
        bool matchesName(const QString &name) const;

        //! Same address and port?
        bool matchesAddressPort(const CServer &server) const;

        //! Same address?
        bool matchesAddress(const QString &address) const;

        //! Get description
        const QString &getDescription() const { return m_description; }

        //! Set description
        void setDescription(const QString &description);

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

        //! Get FSD setup
        const CFsdSetup &getFsdSetup() const { return m_fsdSetup; }

        //! Set FSD setup
        void setFsdSetup(const CFsdSetup &setup) { m_fsdSetup = setup; }

        //! Add send / receive details
        void addSendReceiveDetails(CFsdSetup::SendReceiveDetails sendReceive) { m_fsdSetup.addSendReceiveDetails(sendReceive); }

        //! Remove send / receive details
        void removeSendReceiveDetails(CFsdSetup::SendReceiveDetails sendReceive) { m_fsdSetup.removeSendReceiveDetails(sendReceive); }

        //! A FSD server?
        bool isFsdServer() const;

        //! Set server type
        bool setServerType(ServerType serverType);

        //! Get server type
        ServerType getServerType() const { return static_cast<ServerType>(m_serverType); }

        //! Get voice setup
        const Audio::CVoiceSetup &getVoiceSetup() const { return m_voiceSetup; }

        //! Set voice setup
        void setVoiceSetup(const Audio::CVoiceSetup &setup) { m_voiceSetup = setup; }

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
        QString getServerSessionId(bool onlyConnected) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CServer &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! swift FSD test server
        static const CServer &swiftFsdTestServer(bool withPw = false);

        //! FSC server
        static const CServer &fscFsdServer();

        //! The AFV test server
        static const CServer &afvFsdTestServer();

        //! ES tower server
        static const CServer &esTowerView();

    private:
        QString m_name;
        QString m_description;
        QString m_address;
        int m_port = -1;
        CUser m_user;
        CEcosystem m_ecosystem;
        int m_serverType = static_cast<int>(Unspecified);
        bool m_isAcceptingConnections = true; //!< disable server for connections
        CFsdSetup m_fsdSetup;
        Audio::CVoiceSetup m_voiceSetup;

        BLACK_METACLASS(
            CServer,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(description),
            BLACK_METAMEMBER(address),
            BLACK_METAMEMBER(port),
            BLACK_METAMEMBER(user),
            BLACK_METAMEMBER(fsdSetup),
            BLACK_METAMEMBER(voiceSetup),
            BLACK_METAMEMBER(ecosystem),
            BLACK_METAMEMBER(serverType),
            BLACK_METAMEMBER(isAcceptingConnections),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServer)
Q_DECLARE_METATYPE(BlackMisc::Network::CServer::ServerType)

#endif // guard
