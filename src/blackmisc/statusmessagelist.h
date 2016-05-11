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

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/variant.h"

#include <QJsonArray>
#include <QMetaType>
#include <QtGlobal>

namespace BlackMisc
{

    //! Status messages, e.g. from Core -> GUI
    class BLACKMISC_EXPORT CStatusMessageList :
        public CSequence<CStatusMessage>,
        public ITimestampObjectList<CStatusMessage, CStatusMessageList>,
        public BlackMisc::Mixin::MetaType<CStatusMessageList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CStatusMessageList)

        //! Constructor
        CStatusMessageList() {}

        //! Construct from a base class object.
        CStatusMessageList(const CSequence<CStatusMessage> &other);

        //! Construct from single message
        CStatusMessageList(const CStatusMessage &statusMessage);

        //! Find by type
        CStatusMessageList findByCategory(const CLogCategory &category) const;

        //! Find by severity
        CStatusMessageList findBySeverity(CStatusMessage::StatusSeverity severity) const;

        //! Error messages
        bool hasErrorMessages() const;

        //! Warning messages
        bool hasWarningMessages() const;

        //! Warning or error messages
        bool hasWarningOrErrorMessages() const;

        //! All messages are marked as success
        bool isSuccess() const;

        //! Any message is marked as failure
        bool isFailure() const;

        //! Get all error messages
        CStatusMessageList getErrorMessages() const;

        //! Get all warning and error messages
        CStatusMessageList getWarningAndErrorMessages() const;

        //! Add a category to all messages in the list
        void addCategory(const CLogCategory &category);

        //! Add some categories to all messages in the list
        void addCategories(const CLogCategoryList &categories);

        //! Reset the category of all messages in the list
        void setCategory(const CLogCategory &category);

        //! Reset the categories of all messages in the list
        void setCategories(const CLogCategoryList &categories);

        //! And higher (more critical) severity will be clipped to given severity
        void clampSeverity(CStatusMessage::StatusSeverity severity);

        //! Remove warnings and below
        void removeWarningsAndBelow();

        //! Remove info and below
        void removeInfoAndBelow();

        //! Find worst severity
        CStatusMessage::StatusSeverity worstSeverity() const;

        //! Merge into a single message
        CStatusMessage toSingleMessage() const;

        //! From our database JSON format
        static CStatusMessageList fromDatabaseJson(const QJsonArray &array);
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CStatusMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CStatusMessage>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CStatusMessage>)

#endif // guard
