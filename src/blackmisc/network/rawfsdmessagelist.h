/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_RAWFSDMESSAGELIST_H
#define BLACKMISC_NETWORK_RAWFSDMESSAGELIST_H

#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <initializer_list>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CRawFsdMessage, CRawFsdMessageList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list raw FSD messages.
    class BLACKMISC_EXPORT CRawFsdMessageList :
        public CSequence<CRawFsdMessage>,
        public Mixin::MetaType<CRawFsdMessageList>,
        public ITimestampObjectList<CRawFsdMessage, CRawFsdMessageList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CRawFsdMessageList)
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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CRawFsdMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CRawFsdMessage>)

#endif // guard
