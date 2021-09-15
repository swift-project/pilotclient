/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackgui/models/countrylistmodel.h"
#include "blackmisc/country.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui::Models
{
    CCountryListModel::CCountryListModel(QObject *parent) :
        CListModelDbObjects("CountryListModel", parent)
    {
        CColumn col("country", CCountry::IndexIcon);
        col.setSortPropertyIndex(CCountry::IndexIsoCode);
        m_columns.addColumn(col);
        m_columns.addColumn(CColumn::standardString("ISO2", CCountry::IndexIsoCode));
        m_columns.addColumn(CColumn::standardString("ISO3", CCountry::IndexIso3Code));
        m_columns.addColumn(CColumn::standardString("name", CCountry::IndexName));
        m_columns.addColumn(CColumn::standardString("alias 1", CCountry::IndexAlias1));
        m_columns.addColumn(CColumn::standardString("alias 2", CCountry::IndexAlias2));
        m_columns.addColumn(CColumn::standardString("changed", CCountry::IndexUtcTimestampFormattedYmdhms));

        // default sort order
        this->setSortColumnByPropertyIndex(CCountry::IndexIsoCode);
        m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelCountryList", "cty.");
        (void)QT_TRANSLATE_NOOP("ModelCountryList", "country");
        (void)QT_TRANSLATE_NOOP("ModelCountryList", "ISO2");
        (void)QT_TRANSLATE_NOOP("ModelCountryList", "ISO3");
        (void)QT_TRANSLATE_NOOP("ModelCountryList", "name");
    }
} // ns
