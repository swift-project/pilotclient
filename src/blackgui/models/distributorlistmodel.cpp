/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distributorlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Models
    {
        CDistributorListModel::CDistributorListModel(QObject *parent) :
            CListModelBase("ModelDistributorList", parent)
        {
            this->m_columns.addColumn(CColumn::standardString("key", CDistributor::IndexDbStringKey));
            this->m_columns.addColumn(CColumn::standardString("description", CDistributor::IndexDescription));
            this->m_columns.addColumn(CColumn::standardString("alias1", CDistributor::IndexAlias1));
            this->m_columns.addColumn(CColumn::standardString("alias2", CDistributor::IndexAlias2));

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelDistributorList", "key");
            (void)QT_TRANSLATE_NOOP("ModelDistributorList", "description");
            (void)QT_TRANSLATE_NOOP("ModelDistributorList", "alias1");
            (void)QT_TRANSLATE_NOOP("ModelDistributorList", "alias2");
        }
    } // class
} // namespace
