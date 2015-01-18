/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationlistmodel.h"
#include "blackmisc/voiceroom.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CAtcStationListModel::CAtcStationListModel(AtcStationMode stationMode, QObject *parent) :
            CListModelBase("ViewAtcList", parent), m_stationMode(NotSet)
        {
            this->setStationMode(stationMode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "distance");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "frequency");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "controllername");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "online");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "bookedfrom");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "bookeduntil");
            (void)QT_TRANSLATE_NOOP("ViewAtcList", "voiceroomurl");
        }

        void CAtcStationListModel::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
        {
            if (this->m_stationMode == stationMode) return;
            this->m_stationMode = stationMode;
            this->m_columns.clear();
            switch (stationMode)
            {
            case NotSet:
            case StationsOnline:
                {
                    this->m_columns.addColumn(CColumn::standardString("call", "callsign", { CAtcStation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));
                    CColumn col = CColumn("type", CAtcStation::IndexIcon);
                    col.setSortPropertyIndex({ CAtcStation::IndexCallsign, CCallsign::IndexSuffix });
                    this->m_columns.addColumn(col);
                    this->m_columns.addColumn(CColumn("distance", CAtcStation::IndexDistance, new CAirspaceDistanceFormatter()));
                    this->m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency, new CComFrequencyFormatter()));
                    this->m_columns.addColumn(CColumn::standardString("controllername", { CAtcStation::IndexController, CUser::IndexRealName }));
                    this->m_columns.addColumn(CColumn("from", "booked from", CAtcStation::IndexBookedFrom, new CDateTimeFormatter(CDateTimeFormatter::formatHm())));
                    this->m_columns.addColumn(CColumn("until", "booked until", CAtcStation::IndexBookedUntil, new CDateTimeFormatter(CDateTimeFormatter::formatHm())));
                    this->m_columns.addColumn(CColumn::standardString("voiceroomurl", { CAtcStation::IndexVoiceRoom, CVoiceRoom::IndexUrl }));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CAtcStation::IndexDistance);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            case StationsBooked:
                {
                    this->m_columns.addColumn(CColumn::standardString("call", "callsign", { CAtcStation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));
                    CColumn col = CColumn("type", CAtcStation::IndexIcon);
                    col.setSortPropertyIndex({ CAtcStation::IndexCallsign, CCallsign::IndexSuffix });
                    this->m_columns.addColumn(col);                this->m_columns.addColumn(CColumn("", "on/offline", CAtcStation::IndexIsOnline, new CBoolLedFormatter("online", "offline")));
                    this->m_columns.addColumn(CColumn::standardString("controllername", { CAtcStation::IndexController, CUser::IndexRealName }));
                    this->m_columns.addColumn(CColumn("from", "booked from", CAtcStation::IndexBookedFrom, new CDateTimeFormatter(CDateTimeFormatter::formatYmdhm())));
                    this->m_columns.addColumn(CColumn("until", "booked until", CAtcStation::IndexBookedUntil, new CDateTimeFormatter(CDateTimeFormatter::formatYmdhm())));
                    this->m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency, new CComFrequencyFormatter()));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CAtcStation::IndexBookedFrom);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }

        void CAtcStationListModel::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
        {
            if (station.getCallsign().isEmpty()) return;
            if (added)
            {
                bool c = this->m_container.contains(&CAtcStation::getCallsign, station.getCallsign());
                if (!c) { this->insert(station); }
            }
            else
            {
                beginRemoveRows(QModelIndex(), 0, 0);
                this->removeIf(&CAtcStation::getCallsign, station.getCallsign());
                endRemoveRows();
            }
        }
    }  // namespace
} // namespace
