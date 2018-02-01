/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_RAWFSDMESSAGELIST_H
#define BLACKMISC_NETWORK_RAWFSDMESSAGELIST_H

#include "rawfsdmessage.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QStringList>
#include <QMetaType>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list raw FSD messages.
        class BLACKMISC_EXPORT CRawFsdMessageList :
            public CSequence<CRawFsdMessage>,
            public BlackMisc::Mixin::MetaType<CRawFsdMessageList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CRawFsdMessageList)

            //! Default constructor.
            CRawFsdMessageList();

            //! Construct from a base class object.
            CRawFsdMessageList(const CSequence &other);

            //! Construct from initializer list.
            CRawFsdMessageList(std::initializer_list<CRawFsdMessage> il);

            //! Find by a given list of raw messages which are type
            CRawFsdMessageList findByPacketType(const QString &type) const;

            //! Find by a given list of models by strings
            CRawFsdMessageList findByContainsString(const QString &str) const;
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CRawFsdMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CRawFsdMessageList>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CRawFsdMessageList>)

#endif //guard
