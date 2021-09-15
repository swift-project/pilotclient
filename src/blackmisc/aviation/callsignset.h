/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_CALLSIGNSET_H
#define BLACKMISC_AVIATION_CALLSIGNSET_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QStringList>
#include <tuple>

namespace BlackMisc::Aviation
{
    //! Value object for a set of callsigns.
    class BLACKMISC_EXPORT CCallsignSet : public CCollection<CCallsign>
    {
        using CCollection::CCollection;

    public:
        //! Default constructor
        CCallsignSet();

        //! By string list
        CCallsignSet(const QStringList &callsigns, CCallsign::TypeHint typeHint = CCallsign::NoHint);

        //! Construct from single callsign
        CCallsignSet(const CCallsign &callsign);

        //! Construct from a base class object.
        CCallsignSet(const CCollection<CCallsign> &other);

        //! Contains by string
        bool containsCallsign(const QString &callsign) const;

        //! The callsign strings
        QStringList getCallsignStrings(bool sorted = false) const;

        //! Callsigns as string
        QString getCallsignsAsString(bool sorted = false, const QString &separator = ", ") const;

        //! Register metadata
        static void registerMetadata();
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsignSet)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CCallsign>)

#endif //guard
