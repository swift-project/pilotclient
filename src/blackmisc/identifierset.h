// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_IDENTIFIERSET_H
#define BLACKMISC_IDENTIFIERSET_H

#include "blackmisc/identifier.h"
#include "blackmisc/collection.h"
#include "blackmisc/blackmiscexport.h"

#include <QStringList>
#include <QMetaType>

BLACK_DECLARE_COLLECTION_MIXINS(BlackMisc, CIdentifier, CIdentifierSet)

namespace BlackMisc
{
    /*!
     * Value object encapsulating a set of object identifiers
     */
    class BLACKMISC_EXPORT CIdentifierSet :
        public CCollection<CIdentifier>,
        public Mixin::MetaType<CIdentifierSet>
    {
        using CCollection::CCollection;
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CIdentifierSet)

    public:
        //! Default constructor.
        CIdentifierSet();

        //! Construct from a base class object.
        CIdentifierSet(const CCollection<CIdentifier> &other);

        //! This list contains an identifier which is not contained in other.
        bool containsAnyNotIn(const CIdentifierSet &other) const;

        //! Get a list of identifiers reduced to maximum one per machine.
        //! If there is more than one per machine, it is undefined which one will be added.
        CIdentifierSet getMachinesUnique() const;

        //! Get machine names
        QStringList getMachineNames(bool unique = true, bool sort = true) const;
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CIdentifierSet)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIdentifier>)
// in list: Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CIdentifier>)

#endif // guard
