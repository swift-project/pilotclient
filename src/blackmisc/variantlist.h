/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANTLIST_H
#define BLACKMISC_VARIANTLIST_H

#include "variant.h"
#include "sequence.h"
#include "collection.h"

namespace BlackMisc
{

    /*!
     * Value object encapsulating a list of variants.
     */
    class CVariantList : public CSequence<CVariant>
    {
    public:
        //! Default constructor.
        CVariantList();

        //! Construct from a base class object.
        CVariantList(const CSequence &other);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! \brief Register metadata
        static void registerMetadata();
    };

}

Q_DECLARE_METATYPE(BlackMisc::CVariantList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CVariant>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CVariant>)

#endif
