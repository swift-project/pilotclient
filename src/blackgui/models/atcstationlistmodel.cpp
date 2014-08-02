/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;

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
                this->m_columns.addColumn(CColumn("callsign", CAtcStation::IndexCallsignAsStringAsSet));
                this->m_columns.addColumn(CColumn(CAtcStation::IndexPixmap, true));
                this->m_columns.addColumn(CColumn("distance", CAtcStation::IndexDistance,  Qt::AlignRight  | Qt::AlignVCenter));
                this->m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency,  Qt::AlignRight  | Qt::AlignVCenter));
                this->m_columns.addColumn(CColumn("controllername", CAtcStation::IndexControllerRealName));
                this->m_columns.addColumn(CColumn("bookedfrom", CAtcStation::IndexBookedFrom));
                this->m_columns.addColumn(CColumn("bookeduntil", CAtcStation::IndexBookedUntil));
                this->m_columns.addColumn(CColumn("voiceroomurl", CAtcStation::IndexVoiceRoomUrl));

                // default sort order
                this->setSortColumnByPropertyIndex(CAtcStation::IndexDistance);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case StationsBooked:
                this->m_columns.addColumn(CColumn("callsign", CAtcStation::IndexCallsignAsStringAsSet));
                this->m_columns.addColumn(CColumn(CAtcStation::IndexPixmap, true));
                this->m_columns.addColumn(CColumn("controllername", CAtcStation::IndexControllerRealName));
                this->m_columns.addColumn(CColumn("bookedfrom", CAtcStation::IndexBookedFrom));
                this->m_columns.addColumn(CColumn("bookeduntil", CAtcStation::IndexBookedUntil));
                this->m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency,  Qt::AlignRight  | Qt::AlignVCenter));
                this->m_columns.addColumn(CColumn("online", CAtcStation::IndexIsOnline));

                // default sort order
                this->setSortColumnByPropertyIndex(CAtcStation::IndexBookedFrom);
                this->m_sortOrder = Qt::AscendingOrder;
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
                if (this->m_container.contains(&CAtcStation::getCallsign, station.getCallsign()))
                {
                    this->m_container.replaceIf(&CAtcStation::getCallsign, station.getCallsign(), station);
                }
                else
                {
                    this->insert(station);
                }
            }
            else
            {
                beginRemoveRows(QModelIndex(), 0, 0);
                this->m_container.removeIf(&CAtcStation::getCallsign, station.getCallsign());
                endRemoveRows();
            }
        }
    }
}
