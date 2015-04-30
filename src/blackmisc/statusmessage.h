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

namespace BlackMisc
{

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

        //! Message category
        const CLogCategoryList &getCategories() const { return this->m_categories; }

        //! Human readable category
        QString getHumanReadableCategory() const;

        //! Message severity
        StatusSeverity getSeverity() const { return this->m_severity; }

        //! Message
        QString getMessage() const { return this->m_message; }

        //! Message empty
        bool isEmpty() const { return this->m_message.isEmpty(); }

        //! Message may already have been handled directly
        bool isRedundant() const { return this->m_redundant; }

        //! Info or debug, no warning or error
        bool isSeverityInfoOrLess() const { return this->m_severity == SeverityInfo || this->m_severity == SeverityDebug; }

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

        //! Representing icon
        virtual CIcon toIcon() const override { return convertToIcon(*this); }

        //! Severity as string
        const QString &getSeverityAsString() const;

        //! Severity as string
        static const QString &severityToString(StatusSeverity severity);

        //! Severity as string, if not possible to convert \sa CSeverityInfo
        static StatusSeverity stringToSeverity(const QString &severity);

        //! Severities as strings
        static const QStringList &allSeverityStrings();

        //! \copydoc CValueObject::propertyByIndex
        virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

        //! \copydoc CValueObject::setPropertyByIndex
        virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        //! To HTML
        QString toHtml() const;

        //! Representing icon
        static const CIcon &convertToIcon(const CStatusMessage &statusMessage);

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CStatusMessage)
        CLogCategoryList m_categories;
        StatusSeverity   m_severity = SeverityDebug;
        QString          m_message;
        bool             m_redundant = false;
        mutable QVector<quintptr> m_handledByObjects;
        mutable QString           m_humanReadableCategory; //!< human readable category cache

    };
} // namespace


BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CStatusMessage, (
                                   o.m_categories,
                                   o.m_severity,
                                   o.m_message,
                                   o.m_timestampMSecsSinceEpoch,
                                   o.m_redundant,
                                   attr(o.m_handledByObjects, flags < DisabledForHashing | DisabledForJson | DisabledForComparison | DisabledForMarshalling > ())
                               ))
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)

#endif // guard
