/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackgui/models/serverlistmodel.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"

#include <QtGlobal>

using namespace BlackMisc::Network;

namespace BlackGui::Models
{
    CServerListModel::CServerListModel(QObject *parent) : CListModelBase("ServerListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("name", CServer::IndexName));
        m_columns.addColumn(CColumn::standardString("description", CServer::IndexDescription));
        m_columns.addColumn(CColumn::standardString("address", CServer::IndexAddress));
        m_columns.addColumn(CColumn::standardString("port", CServer::IndexPort));
        m_columns.addColumn(CColumn::standardString("realname", { CServer::IndexUser, CUser::IndexRealName }));
        m_columns.addColumn(CColumn::standardString("userid", { CServer::IndexUser, CUser::IndexId }));
        m_columns.addColumn(CColumn::standardString("eco", { CServer::IndexEcosystem, CEcosystem::IndexSystemString }));
        m_columns.addColumn(CColumn::standardString("type", CServer::IndexServerTypeAsString));

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ServerListModel", "name");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "description");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "address");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "port");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "realname");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "userid");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "eco");
        (void)QT_TRANSLATE_NOOP("ServerListModel", "type");
    }

} // namespace
