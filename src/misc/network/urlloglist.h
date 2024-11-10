// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_URLLOGLIST_H
#define SWIFT_MISC_NETWORK_URLLOGLIST_H

#include "misc/network/urllog.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include <QNetworkReply>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CUrlLog, CUrlLogList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of voice rooms.
    class SWIFT_MISC_EXPORT CUrlLogList :
        public CSequence<CUrlLog>,
        public ITimestampObjectList<CUrlLog, CUrlLogList>,
        public mixin::MetaType<CUrlLogList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CUrlLogList)
        using CSequence::CSequence;

        //! Default constructor.
        CUrlLogList();

        //! Construct from a base class object.
        CUrlLogList(const CSequence &other);

        //! Add a pending URL
        int addPendingUrl(const CUrl &url, int maxNumber = 10);

        //! Add a pending URL
        int addPendingUrl(const CUrl &url, QNetworkReply *nwReply, int maxNumber = 10);

        //! Find pending log entries
        CUrlLogList findPending() const;

        //! Find outdated pending log entries
        CUrlLogList findOutdatedPending(int outdatedOffsetMs) const;

        //! Find log entries with errors (not pending)
        CUrlLogList findErrors() const;

        //! Pending calls
        int sizePending() const;

        //! Any pending calls
        bool hasPending() const;

        //! Any completed calls
        bool hasCompleted() const;

        //! Erroneous calls
        int sizeErrors() const;

        //! Find by id
        CUrlLog findByIdOrDefault(int id) const;

        //! Mark as received
        bool markAsReceived(int id, bool success);

        //! Mark as received
        bool markAsReceived(const QNetworkReply *nwReply, bool success);

        //! Contains the id?
        bool containsId(int id) const;

        //! Maximum response time
        qint64 getMaxResponseTime() const;

        //! Minimum response time
        qint64 getMinResponseTime() const;

        //! Average response time
        qint64 getAverageResponseTime() const;

        //! Summary
        QString getSummary() const;
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::network::CUrlLogList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CUrlLog>)

#endif // guard
