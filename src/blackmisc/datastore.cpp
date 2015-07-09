/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/datastore.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{

    CVariant IDatastoreObjectWithIntegerKey::propertyByIndex(const CPropertyIndex &index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntergerKey: return CVariant::from(this->m_dbKey);
        default:
            break;
        }
        return CVariant();
    }

    void IDatastoreObjectWithIntegerKey::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(variant, index);
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntergerKey:
            this->m_dbKey = variant.toInt();
            break;
        default:
            break;
        }
    }

    bool IDatastoreObjectWithIntegerKey::canHandleIndex(const BlackMisc::CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true;}
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbIntergerKey)) && (i <= static_cast<int>(IndexDbIntergerKey));
    }

    CVariant IDatastoreObjectWithStringKey::propertyByIndex(const CPropertyIndex &index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbStringKey: return CVariant::from(this->m_dbKey);
        default:
            break;
        }
        return CVariant();
    }

    void IDatastoreObjectWithStringKey::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(variant, index);
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbStringKey:
            this->m_dbKey = variant.value<QString>();
            break;
        default:
            break;
        }
    }

    bool IDatastoreObjectWithStringKey::canHandleIndex(const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true;}
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbStringKey)) && (i <= static_cast<int>(IndexDbStringKey));
    }

} // namespace
