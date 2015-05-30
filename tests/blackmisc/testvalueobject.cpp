/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testvalueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    /*
     * Convert to string
     */
    QString CTestValueObject::convertToQString(bool /*i18n*/) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_description);
        return s;
    }

    /*
     * Property by index
     */
    CVariant CTestValueObject::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription:
            return CVariant::fromValue(this->m_description);
        case IndexName:
            return CVariant::fromValue(this->m_name);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    /*
     * Property by index (setter)
     */
    void CTestValueObject::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself()) { (*this) = variant.to<CTestValueObject>(); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription:
            this->setDescription(variant.value<QString>());
            break;
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        default:
            CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

} // namespace
