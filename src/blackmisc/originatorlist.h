/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ORIGINATORLIST_H
#define BLACKMISC_ORIGINATORLIST_H

#include "blackmiscexport.h"
#include "blackmisc/originator.h"
#include "collection.h"
#include "sequence.h"
#include "timestampobjectlist.h"

namespace BlackMisc
{
    //! Value object encapsulating a list of originator objects
    class BLACKMISC_EXPORT COriginatorList :
        public CSequence<BlackMisc::COriginator>,
        public BlackMisc::Mixin::MetaType<COriginatorList>,
        public BlackMisc::ITimestampObjectList<COriginator, COriginatorList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(COriginatorList)

        //! Default constructor.
        COriginatorList();

        //! Construct from a base class object.
        COriginatorList(const CSequence<BlackMisc::COriginator> &other);

    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::COriginatorList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::COriginator>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::COriginator>)

#endif //guard
