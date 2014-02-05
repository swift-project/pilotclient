/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "statusmessagelist.h"
#include "statusmessage.h"

namespace BlackMisc
{
    /*
     * Construct from base class object
     */
    CStatusMessageList::CStatusMessageList(const CSequence<CStatusMessage> &other) :
        CSequence<CStatusMessage>(other)
    { }

    /*
     * Messages by type
     */
    CStatusMessageList CStatusMessageList::findByType(CStatusMessage::StatusType type) const
    {
        CStatusMessageList sm;
        foreach(CStatusMessage message, *this)
        {
            if (message.getType() == type)
            {
                sm.push_back(message);
            }
        }
        return sm;
    }

    /*
     * Messages by severity
     */
    CStatusMessageList CStatusMessageList::findBySeverity(CStatusMessage::StatusSeverity severity) const
    {
        CStatusMessageList sm;
        foreach(CStatusMessage message, *this)
        {
            if (message.getSeverity() == severity)
            {
                sm.push_back(message);
            }
        }
        return sm;
    }

    /*
     * Metadata
     */
    void CStatusMessageList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CStatusMessage>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CStatusMessage>>();
        qRegisterMetaType<BlackMisc::CCollection<CStatusMessage>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CStatusMessage>>();
        qRegisterMetaType<CStatusMessageList>();
        qDBusRegisterMetaType<CStatusMessageList>();
    }
}
