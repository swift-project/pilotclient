// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/variantlist.h"
#include <algorithm>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CVariant, CVariantList)

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
        return containsBy([&](const CVariant &pattern) { return pattern.matches(event); });
    }
} // ns
