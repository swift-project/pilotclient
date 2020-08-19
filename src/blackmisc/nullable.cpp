/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/nullable.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/verify.h"

namespace BlackMisc
{
    INullable::INullable(std::nullptr_t null) : m_isNull(true)
    {
        Q_UNUSED(null);
    }

    bool INullable::canHandleIndex(const CPropertyIndex &index)
    {
        if (index.isEmpty()) { return false; }
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexIsNull)) && (i <= static_cast<int>(IndexIsNull));
    }

    CVariant INullable::propertyByIndex(const CPropertyIndex &index) const
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIsNull:
                return CVariant::fromValue(this->isNull());
            default:
                break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return CVariant::fromValue(m);
    }

    void INullable::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (!index.isEmpty())
        {
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIsNull:
                this->setNull(variant.toBool());
                return;
            default:
                break;
            }
        }
        const QString m = QStringLiteral("Cannot handle index %1").arg(index.toQString());
        BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
    }

    int INullable::comparePropertyByIndex(const CPropertyIndex &index, const INullable &compareValue) const
    {
        Q_UNUSED(index);
        return Compare::compare(this->m_isNull, compareValue.m_isNull);
    }
} // namespace
