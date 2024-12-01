// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOGHISTORY_H
#define SWIFT_MISC_LOGHISTORY_H

#include <QObject>

#include "misc/logpattern.h"
#include "misc/sharedstate/datalink.h"
#include "misc/sharedstate/listjournal.h"
#include "misc/sharedstate/listmutator.h"
#include "misc/sharedstate/listobserver.h"
#include "misc/statusmessagelist.h"

namespace swift::misc
{
    /*!
     * Records all log messages to a list that persists for the lifetime of the application.
     */
    class SWIFT_MISC_EXPORT CLogHistory : public shared_state::CListJournal<CStatusMessageList>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistory(QObject *parent = nullptr);
    };

    /*!
     * Allows distributed insertion of log messages into a central CLogHistory.
     */
    class SWIFT_MISC_EXPORT CLogHistorySource : public shared_state::CListMutator<CStatusMessageList>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistorySource(QObject *parent = nullptr);
    };

    /*!
     * Allows distributed access to the log messages of a central CLogHistory.
     */
    class SWIFT_MISC_EXPORT CLogHistoryReplica : public shared_state::CListObserver<CStatusMessageList, CLogPattern>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("swift.log.history")

    public:
        //! Constructor.
        CLogHistoryReplica(QObject *parent = nullptr);

    signals:
        //! Signal emitted for each new log message.
        void elementAdded(const swift::misc::CStatusMessage &msg);

        //! Signal emitted when the whole history is updated wholesale.
        void elementsReplaced(const swift::misc::CStatusMessageList &msgs);

    private:
        virtual void onElementAdded(const CStatusMessage &msg) override final;
        virtual void onElementsReplaced(const CStatusMessageList &msgs) override final;
    };
} // namespace swift::misc

#endif // SWIFT_MISC_LOGHISTORY_H
