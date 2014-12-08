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
    CStatusMessageList CStatusMessageList::findByCategory(const CLogCategory &category) const
    {
        return this->findBy([ & ](const CStatusMessage &msg) { return msg.getCategories().contains(category); });
    }

    /*
     * Messages by severity
     */
    CStatusMessageList CStatusMessageList::findBySeverity(CStatusMessage::StatusSeverity severity) const
    {
        return this->findBy(&CStatusMessage::getSeverity, severity);
    }

    /*
     * Add category
     */
    void CStatusMessageList::addCategory(const CLogCategory &category)
    {
        for (auto &msg : *this)
        {
            msg.addCategory(category);
        }
    }

    /*
     * Add categories
     */
    void CStatusMessageList::addCategories(const CLogCategoryList &categories)
    {
        for (auto &msg : *this)
        {
            msg.addCategories(categories);
        }
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
        registerMetaValueType<CStatusMessageList>();
    }
}
