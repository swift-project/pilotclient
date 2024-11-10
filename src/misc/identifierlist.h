// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_IDENTIFIERLIST_H
#define SWIFT_MISC_IDENTIFIERLIST_H

#include "misc/identifier.h"
#include "misc/sequence.h"
#include "misc/timestampobjectlist.h"
#include "misc/swiftmiscexport.h"
#include <QMetaType>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CIdentifier, CIdentifierList)

namespace swift::misc
{
    /*!
     * Value object encapsulating a list of object identifiers
     */
    class SWIFT_MISC_EXPORT CIdentifierList :
        public CSequence<CIdentifier>,
        public mixin::MetaType<CIdentifierList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CIdentifierList)
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

Q_DECLARE_METATYPE(swift::misc::CIdentifierList)
// in set: Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CIdentifier>)

#endif // guard
