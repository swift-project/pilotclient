// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/userlistmodel.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

#include "gui/models/columns.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/propertyindexvariantmap.h"

using namespace swift::misc::network;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CUserListModel::CUserListModel(UserMode userMode, QObject *parent) : CListModelBase("ModelUserList", parent), m_userMode(NotSet)
    {
        this->setUserMode(userMode);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelUserList", "callsign");
        (void)QT_TRANSLATE_NOOP("ModelUserList", "realname");
        (void)QT_TRANSLATE_NOOP("ModelUserList", "homebase");
        (void)QT_TRANSLATE_NOOP("ModelUserList", "userid");
        (void)QT_TRANSLATE_NOOP("ModelUserList", "email");
    }

    void CUserListModel::setUserMode(CUserListModel::UserMode userMode)
    {
        if (m_userMode == userMode) return;
        m_userMode = userMode;
        m_columns.clear();
        switch (userMode)
        {
        case NotSet:
        case UserDetailed:
            m_columns.addColumn(CColumn(CUser::IndexIcon));
            m_columns.addColumn(CColumn::standardString("realname", CUser::IndexRealName));
            m_columns.addColumn(CColumn::standardValueObject("callsign", { CUser::IndexCallsign, CCallsign::IndexCallsignString }));
            m_columns.addColumn(CColumn::standardValueObject("hb.", "homebase", { CUser::IndexHomebase, CAirportIcaoCode::IndexString }));
            m_columns.addColumn(CColumn::standardString("userid", CUser::IndexId7Digit));

            // default sort order
            this->setSortColumnByPropertyIndex(CUser::IndexRealName);
            m_sortOrder = Qt::AscendingOrder;
            break;

        case UserShort:
            m_columns.addColumn(CColumn(CUser::IndexIcon));
            m_columns.addColumn(CColumn::standardString("realname", CUser::IndexRealName));
            m_columns.addColumn(CColumn::standardValueObject("callsign", CUser::IndexCallsign));

            // default sort order
            this->setSortColumnByPropertyIndex(CUser::IndexCallsign);
            m_sortOrder = Qt::AscendingOrder;
            break;

        default:
            qFatal("Wrong mode");
            break;
        }
    }
} // namespace swift::gui::models
