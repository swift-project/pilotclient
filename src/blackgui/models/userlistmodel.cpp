/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "userlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CUserListModel::CUserListModel(UserMode userMode, QObject *parent) :
            CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>("ModelUserList", parent), m_userMode(NotSet)
        {
            this->setUserMode(userMode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelUserList", "callsign");
            (void)QT_TRANSLATE_NOOP("ModelUserList", "realname");
            (void)QT_TRANSLATE_NOOP("ModelUserList", "homebase");
            (void)QT_TRANSLATE_NOOP("ModelUserList", "userid");
            (void)QT_TRANSLATE_NOOP("ModelUserList", "email");
        }

        /*
         * Mode
         */
        void CUserListModel::setUserMode(CUserListModel::UserMode userMode)
        {
            if (this->m_userMode == userMode) return;
            this->m_userMode = userMode;
            this->m_columns.clear();
            switch (userMode)
            {
            case NotSet:
            case UserDetailed:
                this->m_columns.addColumn(CColumn(CUser::IndexIcon));
                this->m_columns.addColumn(CColumn::standardString("realname", CUser::IndexRealName));
                this->m_columns.addColumn(CColumn::standardValueObject("callsign", { CUser::IndexCallsign, CCallsign::IndexCallsignString }));
                this->m_columns.addColumn(CColumn::standardValueObject("hb.", "homebase", { CUser::IndexHomebase, CAirportIcao::IndexString }));
                this->m_columns.addColumn(CColumn::standardString("userid", CUser::IndexId));

                // default sort order
                this->setSortColumnByPropertyIndex(CUser::IndexRealName);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case UserShort:
                this->m_columns.addColumn(CColumn(CUser::IndexIcon));
                this->m_columns.addColumn(CColumn::standardString("realname", CUser::IndexRealName));
                this->m_columns.addColumn(CColumn::standardValueObject("callsign", CUser::IndexCallsign));

                // default sort order
                this->setSortColumnByPropertyIndex(CUser::IndexCallsign);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }
    }
}
