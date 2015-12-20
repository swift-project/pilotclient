/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "blackmiscexport.h"
#include "icon.h"
#include "propertyindex.h"
#include "logcategorylist.h"
#include "timestampbased.h"
#include <QReadWriteLock>

namespace BlackMisc
{
    class CStatusException;

    /*!
     * Streamable status message, e.g. from Core -> GUI
     */
    class BLACKMISC_EXPORT CStatusMessage :
        public CValueObject<CStatusMessage>,
        public ITimestampBased
    {
    public:
        //! Status severities
        enum StatusSeverity
        {
            SeverityDebug,
            SeverityInfo,
            SeverityWarning,
            SeverityError
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexCategories = BlackMisc::CPropertyIndex::GlobalIndexCStatusMessage,
            IndexCategoryHumanReadable,
            IndexSeverity,
            IndexSeverityAsString,
            IndexMessage
        };

        //! Constructor
        CStatusMessage() = default;

        //! Copy constructor (because of mutex)
        CStatusMessage(const CStatusMessage &other);

        //! Copy assignment (because of mutex)
        CStatusMessage &operator =(const CStatusMessage &other);

        //! Constructor
        CStatusMessage(const QString &message);

        //! Constructor
        CStatusMessage(StatusSeverity severity, const QString &message);

        //! Constructor
        CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const QString &message);

        //! Construct from a Qt logging triple
        //! \sa QtMessageHandler
        CStatusMessage(QtMsgType type, const QMessageLogContext &context, const QString &message);

        //! Convert to a Qt logging triple
        //! \sa QtMessageHandler
        void toQtLogTriple(QtMsgType *o_type, QString *o_category, QString *o_message) const;

        //! Return a throwable exception object containing this status message.
        CStatusException asException() const;

        //! If message is empty then do nothing, otherwise throw a CStatusException.
        void maybeThrow() const;

        //! Message category
        const CLogCategoryList &getCategories() const { return this->m_categories; }

        //! Human readable category
        QString getHumanReadablePattern() const;

        //! All human readable categories
        QStringList getHumanReadablePatterns() const;

        //! Message severity
        StatusSeverity getSeverity() const { return this->m_severity; }

        //! Info or debug, no warning or error
        bool isSeverityInfoOrLess() const { return this->m_severity == SeverityInfo || this->m_severity == SeverityDebug; }

        //! Warning or above
        bool isWarningOrAbove() const { return this->m_severity == SeverityWarning || this->m_severity == SeverityError; }

        //! Message
        QString getMessage() const { return this->m_message; }

        //! Prepend message
        void prependMessage(const QString &msg);

        //! Append message
        void appendMessage(const QString &msg);

        //! Message empty
        bool isEmpty() const { return this->m_message.isEmpty(); }

        //! Returns true if this message was sent by an instance of class T.
        template <class T>
        bool isFromClass(const T *pointer = nullptr) const
        {
            CLogCategoryList classCategories(pointer);
            return std::all_of(classCategories.begin(), classCategories.end(), [this](const CLogCategory & cat) { return m_categories.contains(cat); });
        }

        //! Mark the message as having been handled by the given object
        void markAsHandledBy(const QObject *object) const;

        //! Returns true if the message was marked as having been handled by the given object
        bool wasHandledBy(const QObject *object) const;

        //! Severity
        void setSeverity(StatusSeverity severity) { this->m_severity = severity; }

        //! Add category
        void addCategory(const CLogCategory &category) { this->m_categories.push_back(category); }

        //! Add categories
        void addCategories(const CLogCategoryList &categories) { this->m_categories.push_back(categories); }

        //! Reset category
        void setCategory(const CLogCategory &category) { this->m_categories = CLogCategoryList { category }; }

        //! Reset categories
        void setCategories(const CLogCategoryList &categories) { this->m_categories = categories; }

        //! Representing icon
        CIcon toIcon() const { return convertToIcon(*this); }

        //! Severity as string
        const QString &getSeverityAsString() const;

        //! Severity as string
        static const QString &severityToString(StatusSeverity severity);

        //! Severity set as string
        static QString severitiesToString(const QSet<StatusSeverity> &severities);

        //! Severity as string, if not possible to convert \sa CSeverityInfo
        static StatusSeverity stringToSeverity(const QString &severity);

        //! Severities as strings
        static const QStringList &allSeverityStrings();

        //! \copydoc CValueObject::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! To HTML
        QString toHtml() const;

        //! \copydoc CValueObject::convertToQString
        QString convertToQString(bool i18n = false) const;

        //! Representing icon
        static const CIcon &convertToIcon(const CStatusMessage &statusMessage);

        //! Representing icon
        static const CIcon &convertToIcon(CStatusMessage::StatusSeverity severity);

        //! Object from JSON
        static CStatusMessage fromDatabaseJson(const QJsonObject &json);

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CStatusMessage)
        CLogCategoryList          m_categories;
        StatusSeverity            m_severity = SeverityDebug;
        QString                   m_message;
        mutable QVector<quintptr> m_handledByObjects;
        mutable QReadWriteLock    m_lock;  //!< lock (because of mutable members)
    };
} // namespace


BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CStatusMessage, (
                                   o.m_categories,
                                   o.m_severity,
                                   o.m_message,
                                   o.m_timestampMSecsSinceEpoch,
                                   attr(o.m_handledByObjects, flags < DisabledForHashing | DisabledForJson | DisabledForComparison | DisabledForMarshalling > ())
                               ))
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage::StatusSeverity)

#endif // guard
