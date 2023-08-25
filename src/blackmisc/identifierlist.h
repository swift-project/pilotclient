// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_IDENTIFIERLIST_H
#define BLACKMISC_IDENTIFIERLIST_H

#include "blackmisc/identifier.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CIdentifier, CIdentifierList)

namespace BlackMisc
{
    /*!
     * Value object encapsulating a list of object identifiers
     */
    class BLACKMISC_EXPORT CIdentifierList :
        public CSequence<CIdentifier>,
        public Mixin::MetaType<CIdentifierList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CIdentifierList)
        using CSequence::CSequence;

        //! Default constructor.
        CIdentifierList();

        //! Construct from a base class object.
        CIdentifierList(const CSequence<CIdentifier> &other);

        //! This list contains an identifier which is not contained in other.
        bool containsAnyNotIn(const CIdentifierList &other) const;

        //! Get a list of identifiers reduced to maximum one per machine.
        //! If there is more than one per machine, it is undefined which one will be added.
        CIdentifierList getMachinesUnique() const;

        //! Get machine names
        QStringList getMachineNames(bool unique = true, bool sort = true) const;

        //! Remove duplicates
        int removeDuplicates();
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CIdentifierList)
// in set: Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIdentifier>)

#endif // guard
