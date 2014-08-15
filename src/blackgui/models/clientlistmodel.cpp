/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "clientlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/icon.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc;
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
            this->m_columns.addColumn(CColumn(CClient::IndexPixmap, true));
            this->m_columns.addColumn(CColumn("callsign", CClient::IndexCallsign));
            this->m_columns.addColumn(CColumn("realname", { CClient::IndexUser, CUser::IndexRealName }));
            this->m_columns.addColumn(CColumn(CClient::IndexVoiceCapabilitiesPixmap, true));
            this->m_columns.addColumn(CColumn("capabilities", CClient::IndexCapabilitiesString));
            this->m_columns.addColumn(CColumn("model", {CClient::IndexModel, CAircraftModel::IndexModelString}));
            this->m_columns.addColumn(CColumn("queried?", {CClient::IndexModel, CAircraftModel::IndexIsQueriedModelString}));
            this->m_columns.addColumn(CColumn("server", CClient::IndexServer));

            // force strings for translation in resource files
            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewClientList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "realname");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "userid");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "model");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "server");
        }
    }
}
