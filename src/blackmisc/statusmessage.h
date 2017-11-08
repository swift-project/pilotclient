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
#include "typetraits.h"

#include <QReadWriteLock>

namespace BlackMisc
{
    class CStatusException;

    namespace Private
    {
        //! Like QString::arg() but accepts a QStringList of args.
        BLACKMISC_EXPORT QString arg(const QString &format, const QStringList &args);
    }

    /*!
     * Status severities
     */
    enum StatusSeverity
    {
        SeverityDebug,
        SeverityInfo,
        SeverityWarning,
        SeverityError
    };

    /*!
     * Base class for CStatusMessage and CLogMessage.
     */
    template <class Derived>
    class CMessageBase
    {
    public:
        //! Default constructor.
        CMessageBase() {}

        //! Construct a message with some specific category.
        explicit CMessageBase(const CLogCategory &category) : m_categories( { category }) {}

        //! Construct a message with some specific categories.
        explicit CMessageBase(const CLogCategoryList &categories) : m_categories(categories) {}

        //! Construct a message with some specific categories.
        CMessageBase(const CLogCategoryList &categories, const CLogCategory &extra) : CMessageBase(categories) { this->addIfNotExisting(extra); }

        //! Construct a message with some specific categories.
        CMessageBase(const CLogCategoryList &categories, const CLogCategoryList &extra) : CMessageBase(categories) { this->addIfNotExisting(extra); }

        //! Set the severity to debug.
        Derived &debug() { return setSeverityAndMessage(SeverityDebug, ""); }

        //! Set the severity to debug, providing a format string.
        Derived &debug(const QString &format) { return setSeverityAndMessage(SeverityDebug, format); }

        //! Set the severity to info, providing a format string.
        Derived &info(const QString &format) { return setSeverityAndMessage(SeverityInfo, format); }

        //! Set the severity to warning, providing a format string.
        Derived &warning(const QString &format) { return setSeverityAndMessage(SeverityWarning, format); }

        //! Set the severity to error, providing a format string.
        Derived &error(const QString &format) { return setSeverityAndMessage(SeverityError, format); }

        //! Set the severity to info, providing a format string, and adding the validation category.
        Derived &validationInfo(const QString &format) { setValidation(); return setSeverityAndMessage(SeverityInfo, format); }

        //! Set the severity to warning, providing a format string, and adding the validation category.
        Derived &validationWarning(const QString &format) { setValidation(); return setSeverityAndMessage(SeverityWarning, format); }

        //! Set the severity to error, providing a format string, and adding the validation category.
        Derived &validationError(const QString &format) { setValidation(); return setSeverityAndMessage(SeverityError, format); }

        //! Streaming operators.
        //! \details If the format string is empty, the message will consist of all streamed values separated by spaces.
        //!          Otherwise, the streamed values will replace the place markers %1, %2, %3... in the format string.
        //! \see QString::arg
        //! @{
        Derived &operator <<(const QString &v) { return arg(v); }
        Derived &operator <<(int v) { return arg(QString::number(v)); }
        Derived &operator <<(uint v) { return arg(QString::number(v)); }
        Derived &operator <<(long v) { return arg(QString::number(v)); }
        Derived &operator <<(ulong v) { return arg(QString::number(v)); }
        Derived &operator <<(qlonglong v) { return arg(QString::number(v)); }
        Derived &operator <<(qulonglong v) { return arg(QString::number(v)); }
        Derived &operator <<(short v) { return arg(QString::number(v)); }
        Derived &operator <<(ushort v) { return arg(QString::number(v)); }
        Derived &operator <<(QChar v) { return arg(v); }
        Derived &operator <<(char v) { return arg(QChar(v)); }
        Derived &operator <<(double v) { return arg(QString::number(v)); }
        template <class T, class = std::enable_if_t<THasToQString<T>::value>>
        Derived &operator <<(const T &v) { return arg(v.toQString()); }
        //! @}

    private:
        void setValidation() { m_categories.remove(CLogCategory::uncategorized()); this->addIfNotExisting(CLogCategory::validation()); }
        Derived &setSeverityAndMessage(StatusSeverity s, const QString &m) { m_message = m; m_severity = s; return derived(); }
        Derived &arg(const QString &value) { m_args.push_back(value); return derived(); }
        Derived &derived() { return static_cast<Derived &>(*this); }

    protected:
        //! Add category if not already existing
        void addIfNotExisting(const CLogCategory &category)
        {
            if (this->m_categories.contains(category)) { return; }
            this->m_categories.push_back(category);
        }

        //! Add categories if not already existing
        void addIfNotExisting(const CLogCategoryList &categories)
        {
            if (this->m_categories.isEmpty())
            {
                this->m_categories.push_back(categories);
            }
            else
            {
                for (const CLogCategory &cat : categories)
                {
                    this->addIfNotExisting(cat);
                }
            }
        }

        //! \private
        //! @{
        QString m_message;
        QStringList m_args;
        CLogCategoryList m_categories = CLogCategoryList { CLogCategory::uncategorized() };
        StatusSeverity m_severity = SeverityDebug;

        QString message() const { return Private::arg(m_message, m_args); }
        //! @}
    };

    /*!
     * Streamable status message, e.g. from Core -> GUI
     */
    class BLACKMISC_EXPORT CStatusMessage :
        public CValueObject<CStatusMessage>,
        public CMessageBase<CStatusMessage>,
        public ITimestampBased
    {
    public:
        //! \copydoc BlackMisc::StatusSeverity
        //! @{
        using StatusSeverity = BlackMisc::StatusSeverity;
        constexpr static auto SeverityDebug = BlackMisc::SeverityDebug;
        constexpr static auto SeverityInfo = BlackMisc::SeverityInfo;
        constexpr static auto SeverityWarning = BlackMisc::SeverityWarning;
        constexpr static auto SeverityError = BlackMisc::SeverityError;
        //! @}

        //! Properties by index
        enum ColumnIndex
        {
            IndexCategoriesAsString = BlackMisc::CPropertyIndex::GlobalIndexCStatusMessage,
            IndexCategoriesHumanReadableAsString,
            IndexCategoryHumanReadableOrTechnicalAsString,
            IndexSeverity,
            IndexSeverityAsString,
            IndexMessage,
            IndexMessageAsHtml
        };

        //! Construct a message with some specific category.
        explicit CStatusMessage(const CLogCategory &category);

        //! Construct a message with some specific categories.
        explicit CStatusMessage(const CLogCategoryList &categories);

        //! Construct a message with some specific categories.
        CStatusMessage(const CLogCategoryList &categories, const CLogCategory &extra);

        //! Construct a message with some specific categories.
        CStatusMessage(const CLogCategoryList &categories, const CLogCategoryList &extra);

        //! Constructor
        CStatusMessage();

        //! Copy constructor (because of mutex)
        CStatusMessage(const CStatusMessage &other);

        //! Copy assignment (because of mutex)
        CStatusMessage &operator =(const CStatusMessage &other);

        //! Destructor.
        ~CStatusMessage() = default;

        //! Constructor
        CStatusMessage(const QString &message);

        //! Constructor
        CStatusMessage(StatusSeverity severity, const QString &message);

        //! Constructor, also a validation messsage can be directly created
        CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const QString &message, bool validation = false);

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

        //! Message categories
        const CLogCategoryList &getCategories() const { return this->m_categories; }

        //! Message categories as string
        QString getCategoriesAsString() const;

        //! Human readable category
        QString getHumanReadablePattern() const;

        //! All human readable categories
        QStringList getHumanReadablePatterns() const;

        //! The human or technical categories
        QString getHumanOrTechnicalCategoriesAsString() const;

        //! Message severity
        StatusSeverity getSeverity() const { return this->m_severity; }

        //! Clip/reduce severity if higher (more critical)
        bool clampSeverity(StatusSeverity severity);

        //! Info or debug, no warning or error
        bool isSeverityInfoOrLess() const { return this->m_severity == SeverityInfo || this->m_severity == SeverityDebug; }

        //! Is this message's severity higher or equal
        bool isSeverityHigherOrEqual(CStatusMessage::StatusSeverity severity) const;

        //! Warning or above
        bool isWarningOrAbove() const { return this->m_severity == SeverityWarning || this->m_severity == SeverityError; }

        //! Operation considered successful
        bool isSuccess() const;

        //! Operation considered unsuccessful
        bool isFailure() const;

        //! Message
        QString getMessage() const { return this->message(); }

        //! Prepend message
        void prependMessage(const QString &msg);

        //! Append message
        void appendMessage(const QString &msg);

        //! Message empty
        bool isEmpty() const { return this->m_message.isEmpty() && this->m_args.isEmpty(); }

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

        //! Add category, avoids duplicates
        void addCategory(const CLogCategory &category) { this->addIfNotExisting(category); }

        //! Adds validation as category
        void addValidationCategory() { this->addCategory(CLogCategory::validation()); }

        //! Add categories, avoids duplicates
        void addCategories(const CLogCategoryList &categories) { this->addIfNotExisting(categories); }

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

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(const CPropertyIndex &index, const CStatusMessage &compareValue) const;

        //! To HTML
        QString toHtml() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
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
        mutable QVector<quintptr> m_handledByObjects;
        mutable QReadWriteLock    m_lock;  //!< lock (because of mutable members)

        BLACK_METACLASS(
            CStatusMessage,
            BLACK_METAMEMBER(categories),
            BLACK_METAMEMBER(severity),
            BLACK_METAMEMBER(message),
            BLACK_METAMEMBER(args),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForHashing | DisabledForComparison)
        );
    };

    // CContainerBase methods implemented out-of-line to avoid circular include
    template <template <class> class C, class T, class CIt>
    CStatusMessage CContainerBase<C, T, CIt>::convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromJson(json);
        }
        catch (const CJsonException &ex)
        {
            return ex.toStatusMessage(categories, prefix);
        }
        return {};
    }

    //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
    template <template <class> class C, class T, class CIt>
    CStatusMessage CContainerBase<C, T, CIt>::convertFromJsonNoThrow(const QString &jsonString, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromJson(jsonString);
        }
        catch (const CJsonException &ex)
        {
            return ex.toStatusMessage(categories, prefix);
        }
        return {};
    }
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage::StatusSeverity)

#endif // guard
