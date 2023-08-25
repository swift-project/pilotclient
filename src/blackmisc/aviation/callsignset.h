// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

BLACK_DECLARE_COLLECTION_MIXINS(BlackMisc::Aviation, CCallsign, CCallsignSet)

namespace BlackMisc::Aviation
{
    //! Value object for a set of callsigns.
    class BLACKMISC_EXPORT CCallsignSet :
        public CCollection<CCallsign>,
        public Mixin::MetaType<CCallsignSet>
    {
        using CCollection::CCollection;
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CCallsignSet)

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

#endif // guard
