/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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

namespace BlackGui::Models
{
    CAirlineIcaoCodeListModel::CAirlineIcaoCodeListModel(QObject *parent) :
        CListModelDbObjects("AircraftIcaoListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("id", CAirlineIcaoCode::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardValueObject("ICAO", CAirlineIcaoCode::IndexAirlineDesignator));

        CColumn col("airline", CAirlineIcaoCode::IndexIcon);
        col.setSortPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn("VA", "virtual airline", CAirlineIcaoCode::IndexIsVirtualAirline, new CBoolIconFormatter("VA", "real airline")));
        m_columns.addColumn(CColumn("mil.", "military (air force)", CAirlineIcaoCode::IndexIsMilitary, new CBoolIconFormatter("military", "civilian")));
        m_columns.addColumn(CColumn("op.", "operating", CAirlineIcaoCode::IndexIsOperating, new CBoolIconFormatter("operating", "ceased")));
        m_columns.addColumn(CColumn::standardValueObject("name", CAirlineIcaoCode::IndexAirlineName));
        m_columns.addColumn(CColumn::standardValueObject("gr.des.", "group designator", CAirlineIcaoCode::IndexGroupDesignator));
        m_columns.addColumn(CColumn::standardValueObject("groupname", CAirlineIcaoCode::IndexGroupName));
        col = CColumn("country", {CAirlineIcaoCode::IndexAirlineCountry, CCountry::IndexIcon});
        col.setSortPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn::standardValueObject("country", "country", CAirlineIcaoCode::IndexAirlineCountry));
        m_columns.addColumn(CColumn::standardString("changed", CAirlineIcaoCode::IndexUtcTimestampFormattedYmdhms));

        // default sort order
        this->setSortColumnByPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
        m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "ICAO");
        (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "operating");
        (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "name");
        (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "country");
        (void)QT_TRANSLATE_NOOP("ModelAirlineIcaoList", "changed");
    }
} // ns
