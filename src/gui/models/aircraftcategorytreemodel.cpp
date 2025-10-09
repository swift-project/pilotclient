// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftcategorytreemodel.h"

#include <QList>
#include <QMap>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/icon.h"
#include "misc/mixin/mixincompare.h"
#include "misc/propertyindex.h"
#include "misc/threadutils.h"
#include "misc/variant.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftCategoryTreeModel::CAircraftCategoryTreeModel(QObject *parent) : QStandardItemModel(parent)
    {
        m_columns.addColumn(CColumn::standardString("id", CAircraftCategory::IndexDbIntegerKey));
        m_columns.addColumn(CColumn::standardString("description", CAircraftCategory::IndexDescription));
        m_columns.addColumn(CColumn::standardString("changed", CAircraftCategory::IndexUtcTimestampFormattedYmdhms));
    }

    void CAircraftCategoryTreeModel::updateContainer(const CAircraftCategoryList &categories)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Wrong thread");

        this->clear();
        if (categories.isEmpty()) { return; }

        m_categories = categories;
        m_categories.sortByLevel();
        QMap<int, QStandardItem *> items;
        this->setColumnCount(m_columns.size() + 1);

        for (const CAircraftCategory &category : m_categories)
        {
            QList<QStandardItem *> categoryRow;

            // ownership of QStandardItem is taken by model
            auto *si = new QStandardItem(category.isAssignable() ? CIcons::paperPlane16() : CIcons::folder16(),
                                         category.getLevelAndName());
            si->setEditable(false);
            categoryRow.push_back(si);

            // add all clumns
            for (const CColumn &column : m_columns.columns())
            {
                const CPropertyIndex i(column.getPropertyIndex());
                const CVariant v(category.propertyByIndex(i));
                const CVariant f = column.getFormatter()->displayRole(v);
                si = new QStandardItem(f.toQString(true));
                si->setEditable(false); // make not editable
                categoryRow.push_back(si);
            } // columns

            // add all items
            if (categoryRow.isEmpty()) { continue; }

            QStandardItem *parent = categoryRow.first();
            if (category.isFirstLevel()) { this->invisibleRootItem()->appendRow(categoryRow); }
            else
            {
                const int p = category.getDepth() - 1;
                Q_ASSERT_X(items[p], Q_FUNC_INFO, "No parent item");
                items[p]->appendRow(categoryRow);
            }
            items.insert(category.getDepth(), parent);
        }
    }

    void CAircraftCategoryTreeModel::clear()
    {
        m_categories.clear();
        QStandardItemModel::clear();
    }
} // namespace swift::gui::models
