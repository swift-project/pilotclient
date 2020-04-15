/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOGHISTORY_H
#define BLACKMISC_LOGHISTORY_H

#include "blackmisc/sharedstate/datalink.h"
#include "blackmisc/sharedstate/listjournal.h"
#include "blackmisc/sharedstate/listmutator.h"
#include "blackmisc/sharedstate/listobserver.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logpattern.h"
#include <QObject>

namespace BlackMisc
{
    /*!
     * Records all log messages to a list that persists for the lifetime of the application.
     */
    class BLACKMISC_EXPORT CLogHistory : public SharedState::CListJournal<CStatusMessageList>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistory(QObject *parent = nullptr);
    };

    /*!
     * Allows distributed insertion of log messages into a central CLogHistory.
     */
    class BLACKMISC_EXPORT CLogHistorySource : public SharedState::CListMutator<CStatusMessageList>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistorySource(QObject *parent = nullptr);
    };

    /*!
     * Allows distributed access to the log messages of a central CLogHistory.
     */
    class BLACKMISC_EXPORT CLogHistoryReplica : public SharedState::CListObserver<CStatusMessageList, CLogPattern>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistoryReplica(QObject *parent = nullptr);

    signals:
        //! Signal emitted for each new log message.
        void elementAdded(const BlackMisc::CStatusMessage &msg);

        //! Signal emitted when the whole history is updated wholesale.
        void elementsReplaced(const BlackMisc::CStatusMessageList &msgs);

    private:
        virtual void onElementAdded(const CStatusMessage &msg) override final;
        virtual void onElementsReplaced(const CStatusMessageList &msgs) override final;
    };
}

#endif
