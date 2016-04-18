/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "orderable.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"
#include <limits>

namespace BlackMisc
{
    IOrderable::IOrderable() { }

    IOrderable::IOrderable(int order) : m_order(order)
    { }

    QString IOrderable::getOrderAsString() const
    {
        if (this->hasValidOrder()) { return QString::number(this->getOrder()); }
        return "-";
    }

    bool IOrderable::hasValidOrder() const
    {
        return this->getOrder() >= 0;
    }

    bool IOrderable::canHandleIndex(const CPropertyIndex &index)
    {
        if (index.isEmpty()) { return false; }
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexOrder)) && (i <= static_cast<int>(IndexOrderString));
    }

    CVariant IOrderable::propertyByIndex(const CPropertyIndex &index) const
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOrder:
                return CVariant::fromValue(this->m_order);
            case IndexOrderString:
                return CVariant::fromValue(this->getOrderAsString());
            default:
                break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        Q_ASSERT_X(false, Q_FUNC_INFO, m.toLocal8Bit().constData());
        return CVariant::fromValue(m);
    }

    void IOrderable::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOrder:
                this->setOrder(variant.toInt());
                return;
            case IndexOrderString:
            default:
                break;
            }
        }
        const QString m = QString("Cannot handle index %1").arg(index.toQString());
        Q_ASSERT_X(false, Q_FUNC_INFO, m.toLocal8Bit().constData());
    }

    int IOrderable::comparePropertyByIndex(const IOrderable &compareValue, const CPropertyIndex &index) const
    {
        Q_UNUSED(index);
        static const int max = std::numeric_limits<int>::max();
        const int o1 = this->hasValidOrder() ? this->getOrder() : max;
        const int o2 = compareValue.hasValidOrder() ? compareValue.getOrder() : max;
        return Compare::compare(o1, o2);
    }
} // namespace
