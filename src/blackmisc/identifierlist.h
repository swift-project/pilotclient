/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_IDENTIFIERLIST_H
#define BLACKMISC_IDENTIFIERLIST_H

#include "blackmisc/identifier.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QMetaType>

namespace BlackMisc
{
    /*!
     * Value object encapsulating a list of object identifiers
     */
    class BLACKMISC_EXPORT CIdentifierList :
        public CSequence<CIdentifier>,
        public Mixin::MetaType<CIdentifierList>,
        public ITimestampObjectList<CIdentifier, CIdentifierList>
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
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CIdentifierList)
// in set: Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIdentifier>)

#endif //guard
