/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "statusmessagelist.h"
#include "statusmessage.h"

namespace BlackMisc
{
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
        qRegisterMetaType<CStatusMessageList>();
        qDBusRegisterMetaType<CStatusMessageList>();
    }
}
