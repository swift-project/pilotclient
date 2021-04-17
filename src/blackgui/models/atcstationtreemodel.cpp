/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/atcstationtreemodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/icon.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/variant.h"
#include "blackmisc/propertyindex.h"

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

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        CAtcStationTreeModel::CAtcStationTreeModel(QObject *parent) : QStandardItemModel(parent)
        { }

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
    }  // namespace
} // namespace
