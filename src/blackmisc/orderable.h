// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_ORDERABLE_H
#define BLACKMISC_ORDERABLE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindexref.h"

#include <QVariant>
#include <QString>

namespace BlackMisc
{
    //! Entity with order attribute (can be manually ordered in views)
    class BLACKMISC_EXPORT IOrderable
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexOrder = CPropertyIndexRef::GlobalIndexIOrderable,
            IndexOrderString
        };

        //! Order
        int getOrder() const { return m_order; }

        //! Order as string
        QString getOrderAsString() const;

        //! Set order
        void setOrder(int order) { m_order = order; }

        //! Valid order set?
        bool hasValidOrder() const;

        //! Any order index
        static bool isAnyOrderIndex(int index);

    protected:
        //! Can given index be handled
        static bool canHandleIndex(CPropertyIndexRef index);

        //! Constructor
        IOrderable();

        //! Constructor
        IOrderable(int order);

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const IOrderable &compareValue) const;

        int m_order = -1; //!< order number
    };
} // namespace

#endif // guard
