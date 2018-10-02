/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/fallthrough.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Models
    {
        CDistributorListModel::CDistributorListModel(QObject *parent) :
            COrderableListModelDbObjects("ModelDistributorList", parent)
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
            if (this->m_distributorMode == distributorMode) { return; }
            this->m_distributorMode = distributorMode;
            this->m_columns.clear();
            switch (distributorMode)
            {
            case NormalWithOrder:
                this->m_columns.addColumn(CColumn::orderColumn());
                BLACK_FALLTHROUGH;
            case NotSet:
            case Normal:
                {
                    this->m_columns.addColumn(CColumn::standardString("key", CDistributor::IndexDbStringKey));
                    this->m_columns.addColumn(CColumn::standardString("description", CDistributor::IndexDescription));
                    this->m_columns.addColumn(CColumn::standardString("alias1", CDistributor::IndexAlias1));
                    this->m_columns.addColumn(CColumn::standardString("alias2", CDistributor::IndexAlias2));
                    this->m_columns.addColumn(CColumn::standardString("sim.", "simulator", { CDistributor::IndexSimulator, CSimulatorInfo::IndexString}));
                    this->m_columns.addColumn(CColumn::standardString("changed", CDistributor::IndexUtcTimestampFormattedYmdhms));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CDistributor::IndexDbStringKey);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            case MinimalWithOrder:
                this->m_columns.addColumn(CColumn::orderColumn());
                BLACK_FALLTHROUGH;
            case Minimal:
                {
                    this->m_columns.addColumn(CColumn::standardString("key", CDistributor::IndexDbStringKey));
                    this->m_columns.addColumn(CColumn::standardString("description", CDistributor::IndexDescription));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CDistributor::IndexDbStringKey);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }
    } // class
} // namespace
