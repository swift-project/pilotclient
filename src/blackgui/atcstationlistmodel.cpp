/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "atcstationlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
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
        case StationsOnline:
            this->m_columns.addColumn(CColumn("callsign", CAtcStation::IndexCallsignAsStringAsSet));
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
            this->m_columns.addColumn(CColumn("controllername", CAtcStation::IndexControllerRealName));
            this->m_columns.addColumn(CColumn("bookedfrom", CAtcStation::IndexBookedFrom));
            this->m_columns.addColumn(CColumn("bookeduntil", CAtcStation::IndexBookedUntil));
            this->m_columns.addColumn(CColumn("frequency", CAtcStation::IndexFrequency,  Qt::AlignRight  | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("online", CAtcStation::IndexIsOnline));

            // default sort order
            this->setSortColumnByPropertyIndex(CAtcStation::IndexBookedFrom);
            this->m_sortOrder = Qt::AscendingOrder;
            break;
        }
    }
}
