/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_URLLOGLIST_H
#define BLACKMISC_NETWORK_URLLOGLIST_H

#include "blackmisc/network/urllog.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QNetworkReply>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class BLACKMISC_EXPORT CUrlLogList :
            public CSequence<CUrlLog>,
            public ITimestampObjectList<CUrlLog, CUrlLogList>,
            public Mixin::MetaType<CUrlLogList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CUrlLogList)
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUrlLogList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUrlLog>)

#endif //guard
