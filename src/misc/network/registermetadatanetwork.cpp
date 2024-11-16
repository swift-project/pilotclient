// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/registermetadatanetwork.h"

// Network headers
#include "misc/network/authenticateduser.h"
#include "misc/network/client.h"
#include "misc/network/clientlist.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/ecosystem.h"
#include "misc/network/ecosystemlist.h"
#include "misc/network/entityflags.h"
#include "misc/network/facilitytype.h"
#include "misc/network/fsdsetup.h"
#include "misc/network/loginmode.h"
#include "misc/network/rawfsdmessage.h"
#include "misc/network/rawfsdmessagelist.h"
#include "misc/network/remotefile.h"
#include "misc/network/remotefilelist.h"
#include "misc/network/role.h"
#include "misc/network/rolelist.h"
#include "misc/network/server.h"
#include "misc/network/serverlist.h"
#include "misc/network/textmessage.h"
#include "misc/network/textmessagelist.h"
#include "misc/network/url.h"
#include "misc/network/urllog.h"
#include "misc/network/urlloglist.h"
#include "misc/network/user.h"
#include "misc/network/userlist.h"
#include "misc/network/voicecapabilities.h"

namespace swift::misc
{
    namespace network
    {
        void registerMetadata()
        {
            CAuthenticatedUser::registerMetadata();
            CConnectionStatus::registerMetadata();
            CClient::registerMetadata();
            qDBusRegisterMetaType<swift::misc::network::CClient::Capability>();
            qDBusRegisterMetaType<swift::misc::network::CClient::Capabilities>();
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
            CUrlLog::registerMetadata();
            CUrlLogList::registerMetadata();
            CUser::registerMetadata();
            CUserList::registerMetadata();
            CVoiceCapabilities::registerMetadata();
        }
    } // namespace network

} // namespace swift::misc
