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
    CAtcListModel::CAtcListModel(QObject *parent) : CListModelBase("ViewAtcList", parent)
    {
        this->m_columns.addColumn(CAtcStation::IndexCallsignAsStringAsSet, "callsign");
        this->m_columns.addColumn(CAtcStation::IndexDistance, "distance", Qt::AlignRight  | Qt::AlignVCenter);
        this->m_columns.addColumn(CAtcStation::IndexFrequency, "frequency", Qt::AlignRight  | Qt::AlignVCenter);
        this->m_columns.addColumn(CAtcStation::IndexControllerRealName, "controllername");
        this->m_columns.addColumn(CAtcStation::IndexIsOnline, "online");
        this->m_columns.addColumn(CAtcStation::IndexBookedFrom, "bookedfrom");
        this->m_columns.addColumn(CAtcStation::IndexBookedUntil, "bookeduntil");

        // default sort order
        this->setSortColumnByPropertyIndex(CAtcStation::IndexDistance);
        this->m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "distance");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "frequency");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "controllername");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "online");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "bookedfrom");
        (void)QT_TRANSLATE_NOOP("ViewAtcList", "bookeduntil");
    }
}
