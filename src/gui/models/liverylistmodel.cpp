// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/liverylistmodel.h"

#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/livery.h"
#include "misc/country.h"
#include "misc/db/datastore.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/timestampbased.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CLiveryListModel::CLiveryListModel(QObject *parent) : CListModelDbObjects("ModelLiveryList", parent)
    {
        m_columns.addColumn(
            CColumn::standardString("id", CLivery::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardString("code", CLivery::IndexCombinedCode));
        m_columns.addColumn(CColumn::standardString("description", CLivery::IndexDescription));
        m_columns.addColumn(CColumn("fuselage", "fuselage color", CLivery::IndexColorFuselage, new CColorFormatter()));
        m_columns.addColumn(CColumn("tail", "tail color", CLivery::IndexColorTail, new CColorFormatter()));
        m_columns.addColumn(
            CColumn("mil.", "military", CLivery::IndexIsMilitary, new CBoolIconFormatter("military", "civil")));
        m_columns.addColumn(CColumn::standardString(
            "des.", "designator", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator }));
        CColumn col = CColumn("airline", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexIcon });
        col.setSortPropertyIndex({ CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountryIso });
        m_columns.addColumn(col);
        m_columns.addColumn(
            CColumn::standardString("name", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));
        col = CColumn("airline country",
                      { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountry, CCountry::IndexIcon });
        col.setSortPropertyIndex({ CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountryIso });
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn::standardString(
            "telephony", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexTelephonyDesignator }));
        m_columns.addColumn(CColumn::standardString("changed", CLivery::IndexUtcTimestampFormattedYmdhms));

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "key");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "description");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "fuselage");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "tail");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "mil.");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "military");
    }
} // namespace swift::gui::models
