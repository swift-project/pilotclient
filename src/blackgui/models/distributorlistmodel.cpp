// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/columns.h"
#include "blackgui/models/distributorlistmodel.h"
#include "misc/db/datastore.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/timestampbased.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace swift::misc::simulation;

namespace BlackGui::Models
{
    CDistributorListModel::CDistributorListModel(QObject *parent) : COrderableListModelDbObjects("ModelDistributorList", parent)
    {
        this->setDistributorMode(Normal);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelDistributorList", "key");
        (void)QT_TRANSLATE_NOOP("ModelDistributorList", "description");
        (void)QT_TRANSLATE_NOOP("ModelDistributorList", "alias1");
        (void)QT_TRANSLATE_NOOP("ModelDistributorList", "alias2");
    }

    void CDistributorListModel::setDistributorMode(CDistributorListModel::DistributorMode distributorMode)
    {
        if (m_distributorMode == distributorMode) { return; }
        m_distributorMode = distributorMode;
        m_columns.clear();
        switch (distributorMode)
        {
        case NormalWithOrder:
            m_columns.addColumn(CColumn::orderColumn());
            [[fallthrough]];
        case NotSet:
        case Normal:
        {
            m_columns.addColumn(CColumn::standardString("key", CDistributor::IndexDbStringKey));
            m_columns.addColumn(CColumn::standardString("description", CDistributor::IndexDescription));
            m_columns.addColumn(CColumn::standardString("alias1", CDistributor::IndexAlias1));
            m_columns.addColumn(CColumn::standardString("alias2", CDistributor::IndexAlias2));
            m_columns.addColumn(CColumn::standardString("sim.", "simulator", { CDistributor::IndexSimulator, CSimulatorInfo::IndexString }));
            m_columns.addColumn(CColumn::standardString("changed", CDistributor::IndexUtcTimestampFormattedYmdhms));

            // default sort order
            this->setSortColumnByPropertyIndex(CDistributor::IndexDbStringKey);
            m_sortOrder = Qt::AscendingOrder;
        }
        break;

        case MinimalWithOrder:
            m_columns.addColumn(CColumn::orderColumn());
            [[fallthrough]];
        case Minimal:
        {
            m_columns.addColumn(CColumn::standardString("key", CDistributor::IndexDbStringKey));
            m_columns.addColumn(CColumn::standardString("description", CDistributor::IndexDescription));

            // default sort order
            this->setSortColumnByPropertyIndex(CDistributor::IndexDbStringKey);
            m_sortOrder = Qt::AscendingOrder;
        }
        break;

        default:
            qFatal("Wrong mode");
            break;
        }
    }
} // namespace
