// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/atcstationtreemodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/callsign.h"
#include "misc/mixin/mixincompare.h"
#include "misc/icon.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/variant.h"
#include "misc/propertyindex.h"

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::network;

namespace swift::gui::models
{
    CAtcStationTreeModel::CAtcStationTreeModel(QObject *parent) : QStandardItemModel(parent)
    {}

    void CAtcStationTreeModel::updateContainer(const CAtcStationList &stations)
    {
        this->clear();
        if (stations.isEmpty()) { return; }

        m_stations = stations.sortedByAtcSuffixSortOrderAndDistance();
        m_stationsBySuffix = m_stations.splitPerSuffix();
        m_suffixes = m_stations.getSuffixes();
        QStandardItemModel::clear();

        int visibleColumns = 0;
        for (const QString &suffix : std::as_const(m_suffixes))
        {
            // ownership of QStandardItem is taken by model
            QStandardItem *typeFolderFirstColumn = new QStandardItem(CCallsign::atcSuffixToIcon(suffix).toQIcon(), suffix);
            typeFolderFirstColumn->setEditable(false);
            this->invisibleRootItem()->appendRow(typeFolderFirstColumn);

            for (const CAtcStation &station : m_stationsBySuffix[suffix])
            {
                int colCount = 0;
                QList<QStandardItem *> stationRow;
                for (const CColumn &column : m_columns.columns())
                {
                    const CPropertyIndex i(column.getPropertyIndex());
                    const CVariant v(station.propertyByIndex(i));

                    QStandardItem *si = nullptr;
                    if (column.getFormatter()->supportsRole(Qt::DecorationRole))
                    {
                        const QIcon icon = column.getFormatter()->decorationRole(v).toPixmap();
                        si = new QStandardItem(icon, QString());
                    }
                    else if (column.getFormatter()->supportsRole(Qt::DisplayRole))
                    {
                        const CVariant f = column.getFormatter()->displayRole(v);
                        si = new QStandardItem(f.toQString(true));
                    }
                    if (!si) { continue; }
                    colCount++;
                    si->setEditable(false); // make not editable
                    stationRow.push_back(si);
                }

                // add all items
                if (stationRow.isEmpty()) { continue; }
                typeFolderFirstColumn->appendRow(stationRow);
                visibleColumns = qMax(visibleColumns, colCount);
            }
        }
        this->setColumnCount(visibleColumns);
    }

    void CAtcStationTreeModel::clear()
    {
        m_stations.clear();
        m_stationsBySuffix.clear();
        m_suffixes.clear();
        QStandardItemModel::clear();
    }

    void CAtcStationTreeModel::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
    {
        Q_UNUSED(station)
        Q_UNUSED(added)
    }
} // namespace
