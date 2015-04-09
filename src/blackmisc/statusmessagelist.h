/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSMESSAGELIST_H
#define BLACKMISC_STATUSMESSAGELIST_H

#include "timestampobjectlist.h"
#include "valueobject.h"
#include "sequence.h"
#include "collection.h"
#include "statusmessage.h"

namespace BlackMisc
{

    /*!
     * Status messages, e.g. from Core -> GUI
     */
    class CStatusMessageList :
        public CSequence<CStatusMessage>,
        public ITimestampObjectList<CStatusMessage, CStatusMessageList>
    {
    public:
        //! Constructor
        CStatusMessageList() {}

        //! Construct from a base class object.
        CStatusMessageList(const CSequence<CStatusMessage> &other);

        //! Find by type
        CStatusMessageList findByCategory(const CLogCategory &category) const;

        //! Find by severity
        CStatusMessageList findBySeverity(CStatusMessage::StatusSeverity severity) const;

        //! Add a category to all messages in the list
        void addCategory(const CLogCategory &category);

        //! Add some categories to all messages in the list
        void addCategories(const CLogCategoryList &categories);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! Register metadata of unit and quantity
        static void registerMetadata();

    };
}

Q_DECLARE_METATYPE(BlackMisc::CStatusMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CStatusMessage>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CStatusMessage>)

#endif // guard
