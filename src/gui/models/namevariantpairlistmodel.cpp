// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/namevariantpairlistmodel.h"

#include <QModelIndex>
#include <Qt>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/mixin/mixincompare.h"
#include "misc/namevariantpair.h"
#include "misc/propertyindexvariantmap.h"

using namespace swift::misc;

namespace swift::gui::models
{
    CNameVariantPairModel::CNameVariantPairModel(bool withIcon, QObject *parent)
        : CListModelBase("ViewNameVariantPairList", parent)
    {
        this->setIconMode(withIcon);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "name");
        (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "value");
    }

    void CNameVariantPairModel::setIconMode(bool withIcon)
    {
        this->m_columns.clear();
        if (withIcon) { this->m_columns.addColumn(CColumn(CNameVariantPair::IndexPixmap)); }
        this->m_columns.addColumn(CColumn::standardString("name", CNameVariantPair::IndexName));
        this->m_columns.addColumn(CColumn("value", CNameVariantPair::IndexVariant, new CDefaultFormatter()));

        // default sort order
        this->setSortColumnByPropertyIndex(CNameVariantPair::IndexName);
        this->m_sortOrder = Qt::AscendingOrder;
    }

    bool CNameVariantPairModel::addOrUpdateByName(const QString &name, const swift::misc::CVariant &value,
                                                  const CIcon &icon, bool skipEqualValues)
    {
        int index = this->getRowIndexForName(name);
        CNameVariantPair pair(name, value, icon);

        if (index < 0)
        {
            // not in the list yet, append
            this->push_back(pair);
            return true;
        }
        else
        {
            // already in list, update
            if (skipEqualValues && this->containsNameValue(name, value)) { return false; }
            this->update(index, pair);
            return true;
        }
    }

    int CNameVariantPairModel::getRowIndexForName(const QString &name) const
    {
        int rowIndex = this->m_container.getIndexForName(name);
        return rowIndex;
    }

    void CNameVariantPairModel::removeByName(const QString &name)
    {
        int rowIndex = this->getRowIndexForName(name);
        if (rowIndex < 0) return;
        QModelIndex i = this->index(rowIndex, 0);
        this->remove(this->at(i));
    }

    bool CNameVariantPairModel::containsName(const QString &name) const { return this->m_container.containsName(name); }

    bool CNameVariantPairModel::containsNameValue(const QString &name, const swift::misc::CVariant &value) const
    {
        int rowIndex = this->getRowIndexForName(name);
        if (rowIndex < 0) { return false; }
        QModelIndex i = this->index(rowIndex, 0);
        const CNameVariantPair cv = this->at(i);
        return value == CVariant::from(cv);
    }
} // namespace swift::gui::models
