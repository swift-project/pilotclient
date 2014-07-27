/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "userlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CUserListModel::CUserListModel(UserMode userMode, QObject *parent) :
            CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>("ViewUserList", parent), m_userMode(NotSet)
        {
            this->setUserMode(userMode);
            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewUserList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewUserList", "realname");
            (void)QT_TRANSLATE_NOOP("ViewUserList", "userid");
            (void)QT_TRANSLATE_NOOP("ViewUserList", "email");
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
                this->m_columns.addColumn(CColumn(CUser::IndexCallsignIcon, true));
                this->m_columns.addColumn(CColumn("realname", CUser::IndexRealName));
                this->m_columns.addColumn(CColumn("callsign", CUser::IndexCallsign));
                this->m_columns.addColumn(CColumn("userid", CUser::IndexId));
                // this->m_columns.addColumn(CUser::IndexEmail, "email");
                break;

            case UserShort:
                this->m_columns.addColumn(CColumn(CUser::IndexCallsignIcon, true));
                this->m_columns.addColumn(CColumn("realname", CUser::IndexRealName));
                this->m_columns.addColumn(CColumn("callsign", CUser::IndexCallsign));
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }
    }
}
