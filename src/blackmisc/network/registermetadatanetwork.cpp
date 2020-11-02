/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadatanetwork.h"
#include "network.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Network
    {
        void registerMetadata()
        {
            CAuthenticatedUser::registerMetadata();
            CConnectionStatus::registerMetadata();
            CClient::registerMetadata();
            qDBusRegisterMetaType<BlackMisc::Network::CClient::Capability>();
            qRegisterMetaTypeStreamOperators<BlackMisc::Network::CClient::Capability>();
            qDBusRegisterMetaType<BlackMisc::Network::CClient::Capabilities>();
            qRegisterMetaTypeStreamOperators<BlackMisc::Network::CClient::Capabilities>();
            CClientList::registerMetadata();
            CEcosystem::registerMetadata();
            CEcosystemList::registerMetadata();
            CEntityFlags::registerMetadata();
            CFacilityType::registerMetadata();
            CFsdSetup::registerMetadata();
            CLoginMode::registerMetadata();
            CRawFsdMessage::registerMetadata();
            CRawFsdMessageList::registerMetadata();
            CRemoteFile::registerMetadata();
            CRemoteFileList::registerMetadata();
            CRole::registerMetadata();
            CRoleList::registerMetadata();
            CServer::registerMetadata();
            CServerList::registerMetadata();
            CTextMessage::registerMetadata();
            CTextMessageList::registerMetadata();
            CUrl::registerMetadata();
            CUrlList::registerMetadata();
            CFailoverUrlList::registerMetadata();
            CUrlLog::registerMetadata();
            CUrlLogList::registerMetadata();
            CUser::registerMetadata();
            CUserList::registerMetadata();
            CVoiceCapabilities::registerMetadata();
            Settings::CNetworkSettings::registerMetadata();

        }
    } // ns
} // ns
