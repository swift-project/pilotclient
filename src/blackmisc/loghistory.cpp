/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/loghistory.h"
#include "blackmisc/loghandler.h"

namespace BlackMisc
{
    CLogHistory::CLogHistory(QObject *parent) : CListJournal(parent)
    {
    }

    CLogHistorySource::CLogHistorySource(QObject *parent) : CListMutator(parent)
    {
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, [this](auto &&...args) {
            this->addElement(args...);
        });
    }

    CLogHistoryReplica::CLogHistoryReplica(QObject *parent) : CListObserver(parent)
    {
    }

    void CLogHistoryReplica::onElementAdded(const CStatusMessage &msg)
    {
        emit elementAdded(msg);
    }

    void CLogHistoryReplica::onElementsReplaced(const CStatusMessageList &msgs)
    {
        emit elementsReplaced(msgs);
    }
}
