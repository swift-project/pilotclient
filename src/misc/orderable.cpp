// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/orderable.h"
#include "misc/comparefunctions.h"
#include "misc/verify.h"

#include <QByteArray>
#include <QtGlobal>
#include <limits>

namespace swift::misc
{
    IOrderable::IOrderable() {}

    IOrderable::IOrderable(int order) : m_order(order)
    {}

    QString IOrderable::getOrderAsString() const
    {
        if (this->hasValidOrder()) { return QString::number(this->getOrder()); }
        return QStringLiteral("-");
    }

    bool IOrderable::hasValidOrder() const
    {
        return this->getOrder() >= 0;
    }

    bool IOrderable::isAnyOrderIndex(int index)
    {
        return (index >= static_cast<int>(IndexOrder)) && (index <= static_cast<int>(IndexOrderString));
    }

    bool IOrderable::canHandleIndex(CPropertyIndexRef index)
    {
        if (index.isEmpty()) { return false; }
        const int i = index.frontCasted<int>();
        return isAnyOrderIndex(i);
    }

    QVariant IOrderable::propertyByIndex(CPropertyIndexRef index) const
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOrder: return QVariant::fromValue(this->m_order);
            case IndexOrderString: return QVariant::fromValue(this->getOrderAsString());
            default: break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return QVariant::fromValue(m);
    }

    void IOrderable::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (!index.isEmpty())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexOrder: this->setOrder(variant.toInt()); return;
            case IndexOrderString:
            default:
                break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int IOrderable::comparePropertyByIndex(CPropertyIndexRef index, const IOrderable &compareValue) const
    {
        Q_UNUSED(index)
        static const int max = std::numeric_limits<int>::max();
        const int o1 = this->hasValidOrder() ? this->getOrder() : max;
        const int o2 = compareValue.hasValidOrder() ? compareValue.getOrder() : max;
        return Compare::compare(o1, o2);
    }
} // namespace
