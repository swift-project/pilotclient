// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_STATUSMESSAGELIST_H
#define SWIFT_MISC_STATUSMESSAGELIST_H

#include "misc/statusmessage.h"
#include "misc/logcategorylist.h"
#include "misc/logcategory.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/timestampobjectlist.h"
#include "misc/orderablelist.h"
#include "misc/swiftmiscexport.h"

#include <QJsonArray>
#include <QMetaType>
#include <QtGlobal>
#include <QMap>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CStatusMessage, CStatusMessageList)

namespace swift::misc
{
    //! Status messages, e.g. from Core -> GUI
    class SWIFT_MISC_EXPORT CStatusMessageList :
        public CSequence<CStatusMessage>,
        public ITimestampObjectList<CStatusMessage, CStatusMessageList>,
        public IOrderableList<CStatusMessage, CStatusMessageList>,
        public mixin::MetaType<CStatusMessageList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CStatusMessageList)
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

Q_DECLARE_METATYPE(swift::misc::CStatusMessageList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CStatusMessage>)

#endif // guard
