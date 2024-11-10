// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircrafticaolistmodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/db/datastore.h"
#include "misc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftIcaoCodeListModel::CAircraftIcaoCodeListModel(QObject *parent) : CListModelDbObjects("AircraftIcaoListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("id", CAircraftIcaoCode::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardString("ICAO", CAircraftIcaoCode::IndexAircraftDesignator));
        m_columns.addColumn(CColumn::standardString("IATA", CAircraftIcaoCode::IndexIataCode));
        m_columns.addColumn(CColumn::standardValueObject("rank", CAircraftIcaoCode::IndexRank, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardString("manufacturer", CAircraftIcaoCode::IndexManufacturer));
        m_columns.addColumn(CColumn::standardString("family", CAircraftIcaoCode::IndexFamily));
        m_columns.addColumn(CColumn::standardString("category", { CAircraftIcaoCode::IndexCategory, CAircraftCategory::IndexPath }));
        // m_columns.addColumn(CColumn::standardString("ICAO desc.", "ICAO description", CAircraftIcaoCode::IndexModelDescription));
        // m_columns.addColumn(CColumn::standardString("IATA desc.", "IATA description", CAircraftIcaoCode::IndexModelIataDescription));
        // m_columns.addColumn(CColumn::standardString("swift desc.", "swift description", CAircraftIcaoCode::IndexModelSwiftDescription));
        m_columns.addColumn(CColumn::standardString("description", "ICAO description", CAircraftIcaoCode::IndexCombinedDescription));
        m_columns.addColumn(CColumn::standardString("type", "combined type", CAircraftIcaoCode::IndexCombinedAircraftType));
        m_columns.addColumn(CColumn::standardString("WTC", "wake turbulence category", CAircraftIcaoCode::IndexWtc));
        m_columns.addColumn(CColumn("mil.", "military", CAircraftIcaoCode::IndexIsMilitary, new CBoolIconFormatter("military", "civil")));
        m_columns.addColumn(CColumn("VTOL", "vertical take-off and landing", CAircraftIcaoCode::IndexIsVtol, new CBoolIconFormatter("VTOL", "non VTOL")));
        m_columns.addColumn(CColumn("leg.", "legacy", CAircraftIcaoCode::IndexIsLegacy, new CBoolIconFormatter("legacy", "operating")));
        m_columns.addColumn(CColumn("real.", "real world aircraft", CAircraftIcaoCode::IndexIsRealworld, new CBoolIconFormatter("real", "non real")));
        m_columns.addColumn(CColumn::standardString("changed", CAircraftIcaoCode::IndexUtcTimestampFormattedYmdhms));

        // default sort order
        this->setSortColumnByPropertyIndex(CAircraftIcaoCode::IndexAircraftDesignator);
        m_sortOrder = Qt::AscendingOrder;
    }
} // ns
