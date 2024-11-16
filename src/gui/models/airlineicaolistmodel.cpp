// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/airlineicaolistmodel.h"

#include <Qt>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/country.h"
#include "misc/db/datastore.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/timestampbased.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAirlineIcaoCodeListModel::CAirlineIcaoCodeListModel(QObject *parent)
        : CListModelDbObjects("AircraftIcaoListModel", parent)
    {
        m_columns.addColumn(
            CColumn::standardString("id", CAirlineIcaoCode::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardValueObject("ICAO", CAirlineIcaoCode::IndexAirlineDesignator));

        CColumn col("airline", CAirlineIcaoCode::IndexIcon);
        col.setSortPropertyIndex(CAirlineIcaoCode::IndexAirlineDesignator);
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn("VA", "virtual airline", CAirlineIcaoCode::IndexIsVirtualAirline,
                                    new CBoolIconFormatter("VA", "real airline")));
        m_columns.addColumn(CColumn("mil.", "military (air force)", CAirlineIcaoCode::IndexIsMilitary,
                                    new CBoolIconFormatter("military", "civilian")));
        m_columns.addColumn(CColumn("op.", "operating", CAirlineIcaoCode::IndexIsOperating,
                                    new CBoolIconFormatter("operating", "ceased")));
        m_columns.addColumn(CColumn::standardValueObject("name", CAirlineIcaoCode::IndexAirlineName));
        m_columns.addColumn(
            CColumn::standardValueObject("gr.des.", "group designator", CAirlineIcaoCode::IndexGroupDesignator));
        m_columns.addColumn(CColumn::standardValueObject("groupname", CAirlineIcaoCode::IndexGroupName));
        col = CColumn("country", { CAirlineIcaoCode::IndexAirlineCountry, CCountry::IndexIcon });
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
} // namespace swift::gui::models
