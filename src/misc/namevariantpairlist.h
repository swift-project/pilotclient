// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NAMEVARIANPAIRTLIST_H
#define SWIFT_MISC_NAMEVARIANPAIRTLIST_H

#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/namevariantpair.h"
#include "misc/sequence.h"
#include "misc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc, CNameVariantPair, CNameVariantPairList)

namespace swift::misc
{
    //! Value object encapsulating a list of name/variant pairs
    //! \note Currently name must be unique
    class SWIFT_MISC_EXPORT CNameVariantPairList :
        public CSequence<CNameVariantPair>,
        public mixin::MetaType<CNameVariantPairList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CNameVariantPairList)
        using CSequence::CSequence;

        //! Default constructor.
        CNameVariantPairList();

        //! Construct from a base class object.
        CNameVariantPairList(const CSequence<CNameVariantPair> &other);

        //! Get name's index, -1 if not found
        int getIndexForName(const QString &name) const;

        //! Contains name
        bool containsName(const QString &name) const;

        //! Get all names
        QStringList getNames(bool sorted = true) const;

        //! Get value
        CNameVariantPair getValue(const QString &name) const;

        //! Get pair's variant value or default if not existing
        CVariant getVariantValue(const QString &name) const;

        //! Get pair's variant value as string (empty if not existing)
        QString getValueAsString(const QString &name) const;

        //! Add value, if name already exists replace (true)
        //! If one is sure(!) the name does not exists, \sa push_back() can be used
        bool addOrReplaceValue(const QString &name, const CVariant &value, const CIcon &icon = CIcon());
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::CNameVariantPairList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CNameVariantPair>)

#endif // guard
