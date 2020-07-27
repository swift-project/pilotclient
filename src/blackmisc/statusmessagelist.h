/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSMESSAGELIST_H
#define BLACKMISC_STATUSMESSAGELIST_H

#include "blackmisc/statusmessage.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/orderablelist.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonArray>
#include <QMetaType>
#include <QtGlobal>
#include <QMap>

namespace BlackMisc
{
    //! Status messages, e.g. from Core -> GUI
    class BLACKMISC_EXPORT CStatusMessageList :
        public CSequence<CStatusMessage>,
        public ITimestampObjectList<CStatusMessage, CStatusMessageList>,
        public IOrderableList<CStatusMessage, CStatusMessageList>,
        public Mixin::MetaType<CStatusMessageList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CStatusMessageList)
        using CSequence::CSequence;

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

        //! Validation category
        void addValidationCategory();

        //! Add some categories to all messages in the list
        void addCategories(const CLogCategoryList &categories);

        //! Convenience function to push back a validation message
        void addValidationMessage(const QString &validationText, CStatusMessage::StatusSeverity severity);

        //! Reset the category of all messages in the list
        void setCategory(const CLogCategory &category);

        //! Reset the categories of all messages in the list
        void setCategories(const CLogCategoryList &categories);

        //! And higher (more critical) severity will be clipped to given severity
        void clampSeverity(CStatusMessage::StatusSeverity severity);

        //! Turn warnings into errors
        void warningToError();

        //! Sort by severity, lowest first
        void sortBySeverity();

        //! Sort by severity, highest first
        void sortBySeverityHighestFirst();

        //! Remove warnings and below
        void removeWarningsAndBelow();

        //! Remove given severity
        void removeSeverity(CStatusMessage::StatusSeverity severity);

        //! Remove info and below
        void removeInfoAndBelow();

        //! Keep latest n status messages
        //! \remark taking timestamp of n-th oldest messages, deleting all older
        int keepLatest(int estimtatedNumber);

        //! Find worst severity
        CStatusMessage::StatusSeverity worstSeverity() const;

        //! Count number of messages per severity
        QMap<int, int> countSeverities() const;

        //! Merge into a single message
        CStatusMessage toSingleMessage() const;

        //! Specialized version to convert to HTML
        QString toHtml(const CPropertyIndexList &indexes = simpleHtmlOutput()) const;

        //! Properties for CStatusMessageList::toHtml
        static const CPropertyIndexList &simpleHtmlOutput();

        //! Properties for CStatusMessageList::toHtml
        static const CPropertyIndexList &timestampHtmlOutput();

        //! Default style sheet which can be used with CStatusMessageList::toHtml
        static const QString &htmlStyleSheet();

        //! From our database JSON format
        static CStatusMessageList fromDatabaseJson(const QJsonArray &array);
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CStatusMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CStatusMessage>)

#endif // guard
