/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/airlineicaolistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/country.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CAirlineIcaoCodeListModel::CAirlineIcaoCodeListModel(QObject *parent) :
            CListModelDbObjects("AircraftIcaoListModel", parent)
        {
            this->m_columns.addColumn(CColumn::standardString("id", CAirlineIcaoCode::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
            this->m_columns.addColumn(CColumn::standardValueObject("ICAO", CAirlineIcaoCode::IndexAirlineDesignator));

            CColumn col("airline", CAirlineIcaoCode::IndexIcon);
            col.setSortPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
            this->m_columns.addColumn(col);
            this->m_columns.addColumn(CColumn("VA", "virtual airline", CAirlineIcaoCode::IndexIsVirtualAirline, new CBoolIconFormatter("VA", "real airline")));
            this->m_columns.addColumn(CColumn("mil.", "military (air force)", CAirlineIcaoCode::IndexIsMilitary, new CBoolIconFormatter("military", "civilian")));
            this->m_columns.addColumn(CColumn("op.", "operating", CAirlineIcaoCode::IndexIsOperating, new CBoolIconFormatter("operating", "ceased")));
            this->m_columns.addColumn(CColumn::standardValueObject("name", CAirlineIcaoCode::IndexAirlineName));
            this->m_columns.addColumn(CColumn::standardValueObject("gr.des.", "group designator", CAirlineIcaoCode::IndexGroupDesignator));
            this->m_columns.addColumn(CColumn::standardValueObject("groupname", CAirlineIcaoCode::IndexGroupName));
            col = CColumn("country", {CAirlineIcaoCode::IndexAirlineCountry, CCountry::IndexIcon});
            col.setSortPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
            this->m_columns.addColumn(col);
            this->m_columns.addColumn(CColumn::standardValueObject("country", "country", CAirlineIcaoCode::IndexAirlineCountry));
            this->m_columns.addColumn(CColumn::standardString("changed", CAirlineIcaoCode::IndexUtcTimestampFormattedYmdhms));

            // default sort order
            this->setSortColumnByPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "ICAO");
            (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "operating");
            (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "name");
            (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "country");
            (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "changed");
        }
    } // ns
} // ns
