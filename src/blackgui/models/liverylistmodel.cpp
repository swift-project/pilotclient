/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/liverylistmodel.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/country.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/timestampbased.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    CLiveryListModel::CLiveryListModel(QObject *parent) : CListModelDbObjects("ModelLiveryList", parent)
    {
        m_columns.addColumn(CColumn::standardString("id", CLivery::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardString("code", CLivery::IndexCombinedCode));
        m_columns.addColumn(CColumn::standardString("description", CLivery::IndexDescription));
        m_columns.addColumn(CColumn("fuselage", "fuselage color", CLivery::IndexColorFuselage, new CColorFormatter()));
        m_columns.addColumn(CColumn("tail", "tail color", CLivery::IndexColorTail, new CColorFormatter()));
        m_columns.addColumn(CColumn("mil.", "military", CLivery::IndexIsMilitary, new CBoolIconFormatter("military", "civil")));
        m_columns.addColumn(CColumn::standardString("des.", "designator", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator }));
        CColumn col = CColumn("airline", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexIcon });
        col.setSortPropertyIndex({ CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountryIso });
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn::standardString("name", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));
        col = CColumn("airline country", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountry, CCountry::IndexIcon });
        col.setSortPropertyIndex({ CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineCountryIso });
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn::standardString("telephony", { CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexTelephonyDesignator }));
        m_columns.addColumn(CColumn::standardString("changed", CLivery::IndexUtcTimestampFormattedYmdhms));

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "key");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "description");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "fuselage");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "tail");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "mil.");
        (void)QT_TRANSLATE_NOOP("ModelLiveryList", "military");
    }
} // namespace
