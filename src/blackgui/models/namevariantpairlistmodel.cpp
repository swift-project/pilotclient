/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpairlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CNameVariantPairModel::CNameVariantPairModel(bool withIcon, QObject *parent) : CListModelBase("ViewNameVariantPairList", parent)
        {
            this->setIconMode(withIcon);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "name");
            (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "value");
        }

        void CNameVariantPairModel::setIconMode(bool withIcon)
        {
            this->m_columns.clear();
            if (withIcon)
            {
                this->m_columns.addColumn(CColumn(CNameVariantPair::IndexPixmap, true));
            }
            this->m_columns.addColumn(CColumn("name", CNameVariantPair::IndexName));
            this->m_columns.addColumn(CColumn("value", CNameVariantPair::IndexVariant));

            // default sort order
            this->setSortColumnByPropertyIndex(CNameVariantPair::IndexName);
            this->m_sortOrder = Qt::AscendingOrder;
        }

        void CNameVariantPairModel::addOrUpdateByName(const QString &name, const QString &value, const CIcon &icon)
        {
            int index = this->getNameRowIndex(name);
            QVariant qv(value);
            CNameVariantPair pair(name, qv, icon);

            if (index < 0)
            {
                // not in the list yet, append
                this->push_back(pair);
            }
            else
            {
                // already in list, update
                this->update(index, pair);
            }
        }

        int CNameVariantPairModel::getNameRowIndex(const QString &name)
        {
            int rowIndex = this->m_container.getNameRowIndex(name);
            return rowIndex;
        }

        void CNameVariantPairModel::removeByName(const QString &name)
        {
            int rowIndex = this->getNameRowIndex(name);
            if (rowIndex < 0) return;
            QModelIndex i = this->index(rowIndex, 0);
            this->remove(this->at(i));
        }
    }
}
