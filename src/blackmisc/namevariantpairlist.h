/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NAMEVARIANPAIRTLIST_H
#define BLACKMISC_NAMEVARIANPAIRTLIST_H

#include "namevariantpair.h"
#include "collection.h"
#include "sequence.h"

namespace BlackMisc
{
    /*!
     * Value object encapsulating a list of name/variant pairs.
     */
    class CNameVariantPairList : public CSequence<CNameVariantPair>
    {
    public:
        //! Default constructor.
        CNameVariantPairList();

        //! Construct from a base class object.
        CNameVariantPairList(const CSequence<CNameVariantPair> &other);

        //! Contains name
        bool containsName(const QString &name);

        //! Get name index
        int getNameRowIndex(const QString &name);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::fromQVariant
        virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! Register metadata
        static void registerMetadata();

    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CNameVariantPairList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CNameVariantPair>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CNameVariantPair>)

#endif //guard
