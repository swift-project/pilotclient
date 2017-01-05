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
            public BlackMisc::ITimestampBased
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
                IndexFsdSetup,
                IndexIsAcceptingConnections
            };

            //! Default constructor.
            CServer() {}

            //! Constructor.
            CServer(const QString &name, const QString &description, const QString &address, int port, const CUser &user, bool isAcceptingConnections = true);

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

            //! Server is accepting connections (allows to disable server temporarily or generally)
            bool isAcceptingConnections() const { return m_isAcceptingConnections; }

            //! Set whether server is accepting connections (allows to disable server temporarily or generally)
            void setIsAcceptingConnections(bool value) { m_isAcceptingConnections = value; }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Address and port?
            bool hasAddressAndPort() const;

            //! Get setup
            const CFsdSetup &getFsdSetup() const { return this->m_fsdSetup; }

            //! Set setup
            void setFsdSetup(const CFsdSetup &setup) { this->m_fsdSetup = setup; }

            //! Connected since
            QDateTime getConnectedSince() const { return this->getUtcTimestamp(); }

            //! Mark as connected since now
            void setConnectedSinceNow() { this->setCurrentUtcTime(); }

            //! Mark as diconnected
            void markAsDisconnected() { this->setTimestampToNull(); }

            //! Is connected?
            bool isConnected() const;

            //! Validate, provide details about issues
            BlackMisc::CStatusMessageList validate() const;

            //! Identifying a session, if not connected empty
            QString getServerSessionId() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const CServer &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

        private:
            QString   m_name;
            QString   m_description;
            QString   m_address;
            int       m_port = -1;
            CUser     m_user;
            CFsdSetup m_fsdSetup;
            bool      m_isAcceptingConnections = true; //!< disable server for connections

            BLACK_METACLASS(
                CServer,
                BLACK_METAMEMBER(name),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(address),
                BLACK_METAMEMBER(port),
                BLACK_METAMEMBER(user),
                BLACK_METAMEMBER(fsdSetup),
                BLACK_METAMEMBER(isAcceptingConnections),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServer)

#endif // guard
