/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
        return this->findBy(&CStatusMessage::getType, type);
    }

    /*
     * Messages by severity
     */
    CStatusMessageList CStatusMessageList::findBySeverity(CStatusMessage::StatusSeverity severity) const
    {
        return this->findBy(&CStatusMessage::getSeverity, severity);
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
