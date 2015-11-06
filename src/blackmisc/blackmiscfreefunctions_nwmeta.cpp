/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "blackmisc/network/network.h"
#include "blackmisc/network/entityflags.h"

/*
 * Metadata for Network
 *
 * In a separate file to workaround a limitation of MinGW:
 * http://stackoverflow.com/q/16596876/1639256
 */
void BlackMisc::Network::registerMetadata()
{
    CAuthenticatedUser::registerMetadata();
    CClient::registerMetadata();
    CClientList::registerMetadata();
    CEntityFlags::registerMetadata();
    CRole::registerMetadata();
    CRoleList::registerMetadata();
    CServer::registerMetadata();
    CServerList::registerMetadata();
    CTextMessage::registerMetadata();
    CTextMessageList::registerMetadata();
    CUrl::registerMetadata();
    CUrlList::registerMetadata();
    CUser::registerMetadata();
    CUserList::registerMetadata();
    CVoiceCapabilities::registerMetadata();
}
