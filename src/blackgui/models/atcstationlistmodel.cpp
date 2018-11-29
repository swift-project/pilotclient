/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"
#include "blackmisc/icon.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexvariantmap.h"

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

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        CAtcStationListModel::CAtcStationListModel(AtcStationMode stationMode, QObject *parent) :
            CListModelCallsignObjects("ModelAtcList", parent)
        {
            this->setStationMode(stationMode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "callsign");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "distance");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "frequency");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "controllername");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "online");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "bookedfrom");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "bookeduntil");
            (void)QT_TRANSLATE_NOOP("ModelAtcList", "voiceroomurl");
        }

        void CAtcStationListModel::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
        {
            if (m_stationMode == stationMode) return;
            m_stationMode = stationMode;
            m_columns.clear();
            switch (stationMode)
            {
            case NotSet:
            case StationsOnline:
                {
                    m_columns.addColumn(CColumn::standardString("cs.", "callsign", { CAtcStation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));
                    CColumn col("type", CAtcStation::IndexIcon);
                    col.setSortPropertyIndex({ CAtcStation::IndexCallsign, CCallsign::IndexSuffix });
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn("distance", CAtcStation::IndexRelativeDistance, new CAirspaceDistanceFormatter()));
                    m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency, new CComFrequencyFormatter()));
                    m_columns.addColumn(CColumn("r.", "in range", CAtcStation::IndexIsInRange, new CBoolIconFormatter("in range", "outside range")));
                    m_columns.addColumn(CColumn("range", CAtcStation::IndexRange, new CAirspaceDistanceFormatter()));
                    m_columns.addColumn(CColumn::standardString("controllername", { CAtcStation::IndexController, CUser::IndexRealName }));
                    m_columns.addColumn(CColumn("from", "booked from", CAtcStation::IndexBookedFrom, new CDateTimeFormatter(CDateTimeFormatter::formatHm())));
                    m_columns.addColumn(CColumn("until", "booked until", CAtcStation::IndexBookedUntil, new CDateTimeFormatter(CDateTimeFormatter::formatHm())));
                    m_columns.addColumn(CColumn::standardString("voiceroomurl", { CAtcStation::IndexVoiceRoom, CVoiceRoom::IndexUrl }));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CAtcStation::IndexRelativeDistance);
                    m_sortOrder = Qt::AscendingOrder;
                }
                break;

            case StationsBooked:
                {
                    m_columns.addColumn(CColumn::standardString("cs.", "callsign", { CAtcStation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));
                    CColumn col = CColumn("type", CAtcStation::IndexIcon);
                    col.setSortPropertyIndex({ CAtcStation::IndexCallsign, CCallsign::IndexSuffix });
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn("", "on/offline", CAtcStation::IndexIsOnline, new CBoolLedFormatter("online", "offline")));
                    m_columns.addColumn(CColumn::standardString("controllername", { CAtcStation::IndexController, CUser::IndexRealName }));
                    m_columns.addColumn(CColumn("from", "booked from", CAtcStation::IndexBookedFrom, new CDateTimeFormatter(CDateTimeFormatter::formatYmdhm())));
                    m_columns.addColumn(CColumn("until", "booked until", CAtcStation::IndexBookedUntil, new CDateTimeFormatter(CDateTimeFormatter::formatYmdhm())));
                    m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency, new CComFrequencyFormatter()));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CAtcStation::IndexBookedFrom);
                    m_sortOrder = Qt::AscendingOrder;
                }
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }

        QStandardItemModel *CAtcStationListModel::toAtcGroupModel() const
        {
            QStandardItemModel *model = new QStandardItemModel();
            if (this->isEmpty()) { return model; }
            model->setColumnCount(4);
            const CAtcStationList atcStations = this->container().sortedByAtcSuffixSortOrderAndDistance();
            const QStringList types = atcStations.getSuffixes();
            for (const QString &type : types)
            {
                // ownership of QStandardItem is taken by model
                QStandardItem *typeFolderFirstColumn = new QStandardItem(CCallsign::atcSuffixToIcon(type).toQIcon(), type);
                typeFolderFirstColumn->setEditable(false);
                QList<QStandardItem *> typeFolderRow { typeFolderFirstColumn };
                model->invisibleRootItem()->appendRow(typeFolderRow);
                CAtcStationList stations = this->container().findBySuffix(type);
                for (const CAtcStation &station : stations)
                {
                    QList<QStandardItem *> stationRow;
                    switch (m_stationMode)
                    {
                    case StationsOnline:
                        stationRow = QList<QStandardItem *>
                        {
                            new QStandardItem(station.getCallsign().toQString()),
                            new QStandardItem(station.getFrequency().valueRoundedWithUnit(CFrequencyUnit::MHz(), 2, true)),
                            new QStandardItem(station.getControllerRealName()),
                            new QStandardItem(station.getRelativeDistance().valueRoundedWithUnit(CLengthUnit::NM(), 1, true))
                        };
                        break;
                    case StationsBooked:
                        stationRow = QList<QStandardItem *>
                        {
                            new QStandardItem(station.getCallsign().toQString()),
                            new QStandardItem(station.getControllerRealName()),
                            new QStandardItem(station.getBookedFromUtc().toString(CDateTimeFormatter::formatYmdhm())),
                            new QStandardItem(station.getBookedUntilUtc().toString(CDateTimeFormatter::formatYmdhm()))
                        };
                        break;
                    case NotSet:
                    default:
                        Q_ASSERT(false);
                        break;
                    }

                    // make not editable
                    for (QStandardItem *si : stationRow)
                    {
                        si->setEditable(false);
                    }

                    // add all items
                    typeFolderFirstColumn->appendRow(stationRow);
                }
            }
            return model;
        }

        void CAtcStationListModel::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
        {
            if (station.getCallsign().isEmpty()) return;
            if (added)
            {
                bool c = m_container.contains(&CAtcStation::getCallsign, station.getCallsign());
                if (!c) { this->insert(station); }
            }
            else
            {
                QModelIndex parent;
                if (rowCount(parent) == 0) { return; }
                beginRemoveRows(parent, 0, 0);
                this->removeIf(&CAtcStation::getCallsign, station.getCallsign());
                endRemoveRows();
            }
        }
    }  // namespace
} // namespace
