/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpairlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CNameVariantPairModel::CNameVariantPairModel(QObject *parent) : CListModelBase("ViewNameVariantPairList", parent)
        {
            this->m_columns.addColumn(CColumn("name", CNameVariantPair::IndexName));
            this->m_columns.addColumn(CColumn("value", CNameVariantPair::IndexVariant));

            // default sort order
            this->setSortColumnByPropertyIndex(CNameVariantPair::IndexName);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "name");
            (void)QT_TRANSLATE_NOOP("ViewNameVariantPairList", "value");
        }
    }
}
