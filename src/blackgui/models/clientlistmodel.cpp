/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "clientlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/icon.h"
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
        CClientListModel::CClientListModel(QObject *parent) :
            CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>("ViewClientList", parent)
        {
            this->m_columns.addColumn(CColumn("", CClient::IndexCallsignIcon));
            this->m_columns.addColumn(CColumn("callsign", CClient::IndexCallsign));
            this->m_columns.addColumn(CColumn("realname", CClient::IndexRealName));
            this->m_columns.addColumn(CColumn("", CClient::IndexVoiceCapabilitiesPixmap));
            this->m_columns.addColumn(CColumn("capabilities", CClient::IndexCapabilitiesString));
            this->m_columns.addColumn(CColumn("model", CClient::IndexQueriedModelString));
            this->m_columns.addColumn(CColumn("server", CClient::IndexServer));

            // force strings for translation in resource files
            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewClientList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "realname");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "userid");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "model");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "server");
        }

        /*
         * Display icons
         */
        QVariant CClientListModel::data(const QModelIndex &modelIndex, int role) const
        {
            // shortcut, fast check
            if (role != Qt::DecorationRole) return CListModelBase::data(modelIndex, role);
            if (this->columnToPropertyIndex(modelIndex.column()) == CClient::IndexCallsignIcon)
            {
                if (role == Qt::DecorationRole)
                {
                    CClient u = this->at(modelIndex);
                    return QVariant(u.toPixmap());
                }
            }
            return CListModelBase::data(modelIndex, role);
        }
    }
}
