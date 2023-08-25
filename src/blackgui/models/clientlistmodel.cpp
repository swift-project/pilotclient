// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/clientlistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/network/user.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackGui::Models
{
    CClientListModel::CClientListModel(QObject *parent) : CListModelBase<CClientList>("ViewClientList", parent)
    {
        m_columns.addColumn(CColumn("client", CClient::IndexIcon));
        m_columns.addColumn(CColumn::standardValueObject("callsign", CClient::IndexCallsign));
        m_columns.addColumn(CColumn::standardString("realname", { CClient::IndexUser, CUser::IndexRealName }));
        m_columns.addColumn(CColumn("vo.", "voice capabilities", CClient::IndexVoiceCapabilitiesIcon, new CPixmapFormatter()));
        m_columns.addColumn(CColumn::standardString("capabilities", CClient::IndexCapabilitiesString));
        m_columns.addColumn(CColumn::standardString("model", CClient::IndexModelString));
        m_columns.addColumn(CColumn("q.?", "queried", { CClient::IndexModelString, CAircraftModel::IndexHasQueriedModelString },
                                    new CBoolIconFormatter("queried", "not queried")));
        m_columns.addColumn(CColumn::standardString("server", CClient::IndexServer));

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewClientList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "realname");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "userid");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "modelstring");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "server");
    }

    QVariant CClientListModel::data(const QModelIndex &index, int role) const
    {
        static const CPropertyIndex ms({ CClient::IndexModelString, CAircraftModel::IndexModelString });
        static const CPropertyIndex qf({ CClient::IndexModelString, CAircraftModel::IndexHasQueriedModelString });
        if (role != Qt::DisplayRole && role != Qt::DecorationRole) { return CListModelBase::data(index, role); }
        const CPropertyIndex pi = modelIndexToPropertyIndex(index);
        if (pi == ms && role == Qt::DisplayRole)
        {
            // no model string for ATC
            const CClient client = this->at(index);
            const bool atc = client.isAtc();
            if (atc) { return QVariant("ATC"); }
        }
        else if (pi == qf && role == Qt::DecorationRole)
        {
            // no symbol for ATC
            const CClient client = this->at(index);
            const bool atc = client.isAtc();
            if (atc) { return QVariant(); }
        }
        return CListModelBase::data(index, role);
    }
} // namespace
