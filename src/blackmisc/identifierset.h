/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
