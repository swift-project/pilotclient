/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackgui/models/userlistmodel.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/propertyindexvariantmap.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
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
} // ns
