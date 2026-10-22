// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/countrylistmodel.h"

#include <Qt>
#include <QtGlobal>

#include "gui/models/columns.h"
#include "misc/country.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/timestampbased.h"

using namespace swift::misc;

namespace swift::gui::models
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
    }
} // namespace swift::gui::models
