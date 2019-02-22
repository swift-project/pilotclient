/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NULLABLE_H
#define BLACKMISC_NULLABLE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    //! Nullable value object
    class BLACKMISC_EXPORT INullable
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexIsNull = CPropertyIndex::GlobalIndexINullable,
        };

        //! Constructor, init to null
        INullable(std::nullptr_t null);

        //! Null?
        bool isNull() const { return m_isNull; }

        //! Null?
        void setNull(bool null) { m_isNull = null; }

        //! Can given index be handled
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

    protected:
        //! Constructor
        INullable() {}

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(const CPropertyIndex &index, const INullable &compareValue) const;

        bool m_isNull = false; //!< null?
    };
} // namespace

#endif // guard
