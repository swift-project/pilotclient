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

#include "blackmiscexport.h"
#include "namevariantpair.h"
#include "collection.h"
#include "sequence.h"

namespace BlackMisc
{
    //! Value object encapsulating a list of name/variant pairs
    //! Currently name must be unique
    class BLACKMISC_EXPORT CNameVariantPairList : public CSequence<CNameVariantPair>
    {
    public:
        //! Default constructor.
        CNameVariantPairList();

        //! Construct from a base class object.
        CNameVariantPairList(const CSequence<CNameVariantPair> &other);

        //! Get name's index, -1 if not found
        int getIndexForName(const QString &name) const;

        //! Contains name
        bool containsName(const QString &name) const;

        //! Get value
        CNameVariantPair getValue(const QString &name) const;

        //! Get pair's variant value or default if not existing
        CVariant getVariantValue(const QString &name) const;

        //! Get pair's variant value as string (empty if not existing)
        QString getValueAsString(const QString &name) const;

        //! Add value, if name already exists replace (true)
        //! If one is sure(!) the name does not exists, \sa push_back() can be used
        bool addOrReplaceValue(const QString &name, const CVariant &value, const CIcon &icon = CIcon());

        //! \copydoc CValueObject::toQVariant
        QVariant toQVariant() const { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        void convertFromQVariant(const QVariant &variant) { BlackMisc::setFromQVariant(this, variant); }

        //! Register metadata
        static void registerMetadata();

    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CNameVariantPairList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CNameVariantPair>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CNameVariantPair>)

#endif //guard
