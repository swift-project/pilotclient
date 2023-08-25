// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKMISC_TOKENBUCKET_H
#define BLACKMISC_TOKENBUCKET_H

#include "blackmisc/blackmiscexport.h"
#include <QDateTime>

namespace BlackMisc
{
    /*!
     * \brief Token bucket algorithm
     * \details This class implements the token bucket algorithm. Tokens as arbitrary unit are added to the bucket at a defined rate.
     *   Token can be consumsed as long as there are enough available. This class can be used to throttle traffic and packet
     *   generation. Each time a packet needs to be generated and sent a token is consumed. If no token is available, consumption
     *   will fail the the packet cannot be sent.
     */
    class BLACKMISC_EXPORT CTokenBucket
    {
    public:
        //! Constructor for given replenishment policy
        CTokenBucket(int capacity, qint64 intervalMs, int numTokensToRefill);

        //! Try to consume a number of tokens
        //! \remark if a current timestamp is already available, it can be passed
        bool tryConsume(int numTokens = 1, qint64 msSinceEpoch = -1);

        //! Number of tokens to refill
        void setNumberOfTokensToRefill(int numTokens);

        //! Set the capacity
        void setCapacity(int capacity);

        //! Tokens/capacity if both are same
        void setCapacityAndTokensToRefill(int numTokens);

        //! Set the interval
        void setInterval(qint64 intervalMs) { m_intervalMs = intervalMs; }

        //! Tokens per second
        int getTokensPerSecond() const;

    private:
        //! Get available tokens since last replenishment.
        //! \note Note that replenishment is implemented lazy.
        //!    This means, tokens will not replenished on regular basis via a running timer,
        //!    but they will be replenished while trying to consume them.
        //! \remark a timestamp can be passed, or "now" is taken
        int getTokens(qint64 msSinceEpoch);

        int m_capacity = 10; //!< Maximum capacity of tokens
        int m_availableTokens = 10; //!< Currently available tokens. The initial value is 10
        int m_numTokensToRefill = 1; //!< Number of tokens to be refilled each interval
        qint64 m_intervalMs = 5000; //!< Refill interval, e.g. every 5 secs
        qint64 m_lastReplenishmentTime = QDateTime::currentMSecsSinceEpoch(); //!< Last time
    };
} // ns

#endif
