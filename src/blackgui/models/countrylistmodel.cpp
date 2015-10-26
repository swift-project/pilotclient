/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "countrylistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CCountryListModel::CCountryListModel(QObject *parent) :
            CListModelBase("CountryListModel", parent)
        {
            CColumn col("country", CCountry::IndexIcon);
            col.setSortPropertyIndex(CCountry::IndexIsoCode);
            this->m_columns.addColumn(col);
            this->m_columns.addColumn(CColumn::standardString("ISO", CCountry::IndexIsoCode));
            this->m_columns.addColumn(CColumn::standardString("name", CCountry::IndexName));
            this->m_columns.addColumn(CColumn::standardString("changed", CCountry::IndexUtcTimestampFormattedYmdhms));

            // default sort order
            this->setSortColumnByPropertyIndex(CCountry::IndexIsoCode);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelCountryList", "cty.");
            (void)QT_TRANSLATE_NOOP("ModelCountryList", "country");
            (void)QT_TRANSLATE_NOOP("ModelCountryList", "ISO");
            (void)QT_TRANSLATE_NOOP("ModelCountryList", "name");
        }
    } // ns
} // ns
