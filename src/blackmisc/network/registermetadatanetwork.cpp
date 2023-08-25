// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/registermetadatanetwork.h"

// Network headers
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/ecosystem.h"
#include "blackmisc/network/ecosystemlist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/facilitytype.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/network/rawfsdmessagelist.h"
#include "blackmisc/network/role.h"
#include "blackmisc/network/rolelist.h"
#include "blackmisc/network/remotefile.h"
#include "blackmisc/network/remotefilelist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/urllog.h"
#include "blackmisc/network/urlloglist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"

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
        }
    }

} // ns
