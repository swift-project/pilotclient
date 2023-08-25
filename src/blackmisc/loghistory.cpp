// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
