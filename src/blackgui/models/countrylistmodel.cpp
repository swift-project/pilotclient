// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/columns.h"
#include "blackgui/models/countrylistmodel.h"
#include "misc/country.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc;

namespace BlackGui::Models
{
    CCountryListModel::CCountryListModel(QObject *parent) : CListModelDbObjects("CountryListModel", parent)
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
