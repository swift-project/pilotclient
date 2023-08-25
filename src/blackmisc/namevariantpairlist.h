// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NAMEVARIANPAIRTLIST_H
#define BLACKMISC_NAMEVARIANPAIRTLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/namevariantpair.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CNameVariantPair, CNameVariantPairList)

namespace BlackMisc
{
    //! Value object encapsulating a list of name/variant pairs
    //! \note Currently name must be unique
    class BLACKMISC_EXPORT CNameVariantPairList :
        public CSequence<CNameVariantPair>,
        public Mixin::MetaType<CNameVariantPairList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CNameVariantPairList)
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

Q_DECLARE_METATYPE(BlackMisc::CNameVariantPairList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CNameVariantPair>)

#endif // guard
