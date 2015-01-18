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
#include "blackmisc/icons.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
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
            this->m_columns.addColumn(CColumn("client", CClient::IndexIcon));
            this->m_columns.addColumn(CColumn::standardValueObject("callsign", CClient::IndexCallsign));
            this->m_columns.addColumn(CColumn::standardString("realname", { CClient::IndexUser, CUser::IndexRealName }));
            this->m_columns.addColumn(CColumn("vo.", "voice capabilities", CClient::IndexVoiceCapabilitiesIcon,new CPixmapFormatter()));
            this->m_columns.addColumn(CColumn::standardString("capabilities", CClient::IndexCapabilitiesString));
            this->m_columns.addColumn(CColumn::standardString("model", {CClient::IndexModel, CAircraftModel::IndexModelString}));
            this->m_columns.addColumn(CColumn("q.?", "queried", {CClient::IndexModel, CAircraftModel::IndexHasQueriedModelString},
                                              new CBoolIconFormatter(CIcons::StandardIconTick16, CIcons::StandardIconCross16, "queried", "not queried")));
            this->m_columns.addColumn(CColumn::standardString("server", CClient::IndexServer));

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewClientList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "realname");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "userid");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "model");
            (void)QT_TRANSLATE_NOOP("ViewClientList", "server");
        }

        QVariant CClientListModel::data(const QModelIndex &index, int role) const
        {
            static const CPropertyIndex ms( {CClient::IndexModel, CAircraftModel::IndexModelString});
            static const CPropertyIndex qf( {CClient::IndexModel, CAircraftModel::IndexHasQueriedModelString});
            if (role != Qt::DisplayRole && role != Qt::DecorationRole) { return CListModelBase::data(index, role); }
            CPropertyIndex pi = modelIndexToPropertyIndex(index);
            if (pi == ms && role == Qt::DisplayRole)
            {
                // no model string for ATC
                const CClient client = this->at(index);
                bool atc = client.isAtc();
                if (atc)
                {
                    return QVariant("ATC");
                }
            }
            else if (pi == qf && role == Qt::DecorationRole)
            {
                // no symbol for ATC
                const CClient client = this->at(index);
                bool atc = client.isAtc();
                if (atc)
                {
                    return QVariant();
                }
            }
            return CListModelBase::data(index, role);
        }
    } // namespace
} // namespace
