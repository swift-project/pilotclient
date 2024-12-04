// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_RAWFSDMESSAGELIST_H
#define SWIFT_MISC_NETWORK_RAWFSDMESSAGELIST_H

#include <initializer_list>

#include <QMetaType>

#include "misc/collection.h"
#include "misc/network/rawfsdmessage.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CRawFsdMessage, CRawFsdMessageList)

namespace swift::misc::network
{
    //! Value object encapsulating a list raw FSD messages.
    class SWIFT_MISC_EXPORT CRawFsdMessageList :
        public CSequence<CRawFsdMessage>,
        public mixin::MetaType<CRawFsdMessageList>,
        public ITimestampObjectList<CRawFsdMessage, CRawFsdMessageList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CRawFsdMessageList)
        using CSequence::CSequence;

        //! Default constructor.
        CRawFsdMessageList();

        //! Construct from a base class object.
        CRawFsdMessageList(const CSequence &other);

        //! Find by a given list of raw messages which are type
        CRawFsdMessageList findByPacketType(const QString &type) const;

        //! Find by a given list of models by strings
        CRawFsdMessageList findByContainsString(const QString &str) const;
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CRawFsdMessageList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CRawFsdMessage>)

#endif // SWIFT_MISC_NETWORK_RAWFSDMESSAGELIST_H
