// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/serverlistmodel.h"

#include <QtGlobal>

#include "gui/models/columns.h"
#include "misc/network/server.h"
#include "misc/network/user.h"

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
    }

} // namespace swift::gui::models
