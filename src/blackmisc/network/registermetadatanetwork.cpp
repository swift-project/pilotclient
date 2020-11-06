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
    }

#if defined(Q_OS_WIN) && defined(Q_CC_CLANG)
    namespace Private
    {
        template void maybeRegisterMetaListConvert<Network::CClientList>(int);
        template void maybeRegisterMetaListConvert<Network::CEcosystemList>(int);
        template void maybeRegisterMetaListConvert<Network::CRawFsdMessageList>(int);
        template void maybeRegisterMetaListConvert<Network::CRemoteFileList>(int);
        template void maybeRegisterMetaListConvert<Network::CRoleList>(int);
        template void maybeRegisterMetaListConvert<Network::CServerList>(int);
        template void maybeRegisterMetaListConvert<Network::CTextMessageList>(int);
        template void maybeRegisterMetaListConvert<Network::CUrlList>(int);
        template void maybeRegisterMetaListConvert<Network::CFailoverUrlList>(int);
        template void maybeRegisterMetaListConvert<Network::CUrlLogList>(int);
        template void maybeRegisterMetaListConvert<Network::CUserList>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CClient>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CEcosystem>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CRawFsdMessage>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CRemoteFile>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CRole>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CServer>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CTextMessage>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CUrl>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CUrlLog>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Network::CUser>>(int);
    } // ns
#endif

} // ns
