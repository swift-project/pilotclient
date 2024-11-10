// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/columns.h"
#include "gui/models/serverlistmodel.h"
#include "misc/network/server.h"
#include "misc/network/user.h"

#include <QtGlobal>

using namespace swift::misc::network;

namespace swift::gui::models
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
