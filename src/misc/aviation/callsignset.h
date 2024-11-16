// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_CALLSIGNSET_H
#define SWIFT_MISC_AVIATION_CALLSIGNSET_H

#include <tuple>

#include <QMetaType>
#include <QStringList>

#include "misc/aviation/callsign.h"
#include "misc/collection.h"
#include "misc/mixin/mixincompare.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_COLLECTION_MIXINS(swift::misc::aviation, CCallsign, CCallsignSet)

namespace swift::misc::aviation
{
    //! Value object for a set of callsigns.
    class SWIFT_MISC_EXPORT CCallsignSet : public CCollection<CCallsign>, public mixin::MetaType<CCallsignSet>
    {
        using CCollection::CCollection;
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CCallsignSet)

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
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CCallsignSet)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CCallsign>)

#endif // guard
