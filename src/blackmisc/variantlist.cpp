/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/variantlist.h"
#include <algorithm>
#include <iterator>

namespace BlackMisc
{
    //! \private
    extern int qMetaTypeId_CVariantList; // defined in variant.cpp

    CVariantList::CVariantList(const CSequence &other) : CSequence(other)
    {}

    CVariantList::CVariantList(const QVariantList &other)
    {
        std::copy(other.begin(), other.end(), std::back_inserter(*this));
    }

    CVariantList::CVariantList(QVariantList &&other)
    {
        std::move(other.begin(), other.end(), std::back_inserter(*this));
    }

    void CVariantList::registerMetadata()
    {
        Mixin::MetaType<CVariantList>::registerMetadata();
        QMetaType::registerConverter<CVariantList, QVector<CVariant>>([](const CVariantList &list) { return list.toVector(); });
        QMetaType::registerConverter<QVector<CVariant>, CVariantList>([](const QVector<CVariant> &list) { return CSequence(list); });
        qMetaTypeId_CVariantList = qMetaTypeId<CVariantList>();
    }

    bool CVariantList::matches(const CVariant &event) const
    {
        return containsBy([ & ](const CVariant &pattern) { return pattern.matches(event); });
    }
} // ns
