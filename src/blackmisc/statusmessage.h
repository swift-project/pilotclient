/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "blackmisc/logcategorylist.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/orderable.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/typetraits.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/stringutils.h"

#include <QReadWriteLock>

namespace BlackMisc
{
    class CStatusException;

    namespace Private
    {
        //! Like QString::arg() but for QStringView.
        BLACKMISC_EXPORT QString arg(QStringView format, const QStringList &args);
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
     * Special-purpose string class used by CMessageBase.
     *
     * Wraps a QStringView that can be constructed from a UTF-16 string literal or from a QString.
     * If constructed from a QString, the QString is stored to prevent a dangling pointer.
     */
    class CStrongStringView :
        public Mixin::MetaType<CStrongStringView>,
        public Mixin::EqualsByCompare<CStrongStringView>,
        public Mixin::LessThanByCompare<CStrongStringView>,
        public Mixin::DBusOperators<CStrongStringView>,
        public Mixin::DataStreamOperators<CStrongStringView>,
        public Mixin::JsonOperators<CStrongStringView>,
        public Mixin::String<CStrongStringView>
    {
    public:
        //! Default constructor.
        CStrongStringView() = default;

        //! Construct from a UTF-16 character array.
        template <size_t N>
        CStrongStringView(const char16_t (&string)[N]) : m_view(string) {}

        //! Construct from a QString.
        CStrongStringView(const QString &string) : m_string(string), m_view(m_string) {}

        //! Construct from a QStringView. Explicit because it could be dangerous if used without care.
        explicit CStrongStringView(QStringView view) : m_view(view) {}

        //! Deleted constructor.
        CStrongStringView(const char *) = delete;

        //! Copy constructor.
        CStrongStringView(const CStrongStringView &other) { *this = other; }

        //! Copy assignment operator.
        CStrongStringView &operator =(const CStrongStringView &other)
        {
            if (other.isOwning()) { m_view = m_string = other.m_string; } else { m_view = other.m_view; m_string.clear(); }
            return *this;
        }

        //! Destructor.
        ~CStrongStringView() = default;

        //! String is empty.
        bool isEmpty() const { return view().isEmpty(); }

        //! Does it own its string data?
        bool isOwning() const { return !m_string.isNull(); } // important distinction between isNull and isEmpty

        //! Return as a QStringView.
        QStringView view() const { return m_view; }

        //! Return a copy as a QString.
        QString convertToQString(bool i18n = false) const { Q_UNUSED(i18n); return isOwning() ? m_string : m_view.toString(); }

        //! Compare two strings.
        friend int compare(const CStrongStringView &a, const CStrongStringView &b) { return a.m_view.compare(b.m_view); }

        //! Hash value.
        friend uint qHash(const CStrongStringView &obj, uint seed = 0) { return ::qHash(obj.m_view, seed); }

        //! DBus marshalling.
        //! @{
        void marshallToDbus(QDBusArgument &arg) const { arg << toQString(); }
        void unmarshallFromDbus(const QDBusArgument &arg) { arg >> m_string; m_view = m_string; }
        //! @}

        //! QDataStream marshalling.
        //! @{
        void marshalToDataStream(QDataStream &stream) const { stream << toQString(); }
        void unmarshalFromDataStream(QDataStream &stream) { stream >> m_string; m_view = m_string; }
        //! @}

        //! JSON conversion.
        //! @{
        QJsonObject toJson() const { QJsonObject json; json.insert(QStringLiteral("value"), toQString()); return json; }
        void convertFromJson(const QJsonObject &json) { *this = json.value(QLatin1String("value")).toString(); }
        //! @}

    private:
        QString m_string;
        QStringView m_view;
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
        explicit CMessageBase(const CLogCategory &category) : m_categories({ category }) {}

        //! Construct a message with some specific categories.
        explicit CMessageBase(const CLogCategoryList &categories) : m_categories(categories) {}

        //! Construct a message with some specific categories.
        CMessageBase(const CLogCategoryList &categories, const CLogCategory &extra) : CMessageBase(categories) { this->addIfNotExisting(extra); }

        //! Construct a message with some specific categories.
        CMessageBase(const CLogCategoryList &categories, const CLogCategoryList &extra) : CMessageBase(categories) { this->addIfNotExisting(extra); }

        //! Set the severity and format string.
        //! @{
        template <size_t N>
        Derived &log(StatusSeverity s, const char16_t (&m)[N]) { m_message = m; m_severity = s; return derived(); }
        Derived &log(StatusSeverity s, const QString &m) { m_message = m; m_severity = s; return derived(); }
        //! @}

        //! Set the severity to debug.
        Derived &debug() { return log(SeverityDebug, QString()); }

        //! Set the severity to debug, providing a format string.
        //! @{
        template <size_t N>
        Derived &debug(const char16_t (&format)[N]) { return log(SeverityDebug, format); }
        Derived &debug(const QString &format) { return log(SeverityDebug, format); }
        //! @}

        //! Set the severity to info, providing a format string.
        //! @{
        template <size_t N>
        Derived &info(const char16_t (&format)[N]) { return log(SeverityInfo, format); }
        Derived &info(const QString &format) { return log(SeverityInfo, format); }
        //! @}

        //! Set the severity to warning, providing a format string.
        //! @{
        template <size_t N>
        Derived &warning(const char16_t (&format)[N]) { return log(SeverityWarning, format); }
        Derived &warning(const QString &format) { return log(SeverityWarning, format); }
        //! @}

        //! Set the severity to error, providing a format string.
        //! @{
        template <size_t N>
        Derived &error(const char16_t (&format)[N]) { return log(SeverityError, format); }
        Derived &error(const QString &format) { return log(SeverityError, format); }
        //! @}

        //! Set the severity to s, providing a format string, and adding the validation category.
        //! @{
        template <size_t N>
        Derived &validation(StatusSeverity s, const char16_t (&format)[N]) { setValidation(); return log(s, format); }
        Derived &validation(StatusSeverity s, const QString &format) { setValidation(); return log(s, format); }
        //! @}

        //! Set the severity to info, providing a format string, and adding the validation category.
        //! @{
        template <size_t N>
        Derived &validationInfo(const char16_t (&format)[N]) { setValidation(); return log(SeverityInfo, format); }
        Derived &validationInfo(const QString &format) { setValidation(); return log(SeverityInfo, format); }
        //! @}

        //! Set the severity to warning, providing a format string, and adding the validation category.
        //! @{
        template <size_t N>
        Derived &validationWarning(const char16_t (&format)[N]) { setValidation(); return log(SeverityWarning, format); }
        Derived &validationWarning(const QString &format) { setValidation(); return log(SeverityWarning, format); }
        //! @}

        //! Set the severity to error, providing a format string, and adding the validation category.
        //! @{
        template <size_t N>
        Derived &validationError(const char16_t (&format)[N]) { setValidation(); return log(SeverityError, format); }
        Derived &validationError(const QString &format) { setValidation(); return log(SeverityError, format); }
        //! @}

        //! Deleted methods to avoid accidental implicit conversion from Latin-1 or UTF-8 string literals.
        //! @{
        Derived &log(StatusSeverity, const char *) = delete;
        Derived &debug(const char *) = delete;
        Derived &info(const char *) = delete;
        Derived &warning(const char *) = delete;
        Derived &error(const char *) = delete;
        Derived &validation(StatusSeverity, const char *) = delete;
        Derived &validationInfo(const char *) = delete;
        Derived &validationWarning(const char *) = delete;
        Derived &validationError(const char *) = delete;
        //! @}

        //! Streaming operators.
        //! \details If the format string is empty, the message will consist of all streamed values separated by spaces.
        //!          Otherwise, the streamed values will replace the place markers %1, %2, %3... in the format string.
        //! \see QString::arg
        //! @{
        template <class T, std::enable_if_t<TParameter<std::decay_t<T>>::passBy == ParameterPassBy::Value, int> = 0>
        Derived &operator <<(T v) { return arg(TString<T>::toQString(v)); }
        template <class T, std::enable_if_t<TParameter<std::decay_t<T>>::passBy == ParameterPassBy::ConstRef, int> = 0>
        Derived &operator <<(const T &v) { return arg(TString<T>::toQString(v)); }
        //! @}

        //! Message empty
        bool isEmpty() const { return this->m_message.isEmpty() && this->m_args.isEmpty(); }

    private:
        void setValidation() { m_categories.remove(CLogCategories::uncategorized()); this->addIfNotExisting(CLogCategories::validation()); }
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

    protected:
        //! \private
        //! @{
        CStrongStringView m_message;
        QStringList m_args;
        CLogCategoryList m_categories = CLogCategoryList { CLogCategories::uncategorized() };
        StatusSeverity m_severity = SeverityDebug;

        QString message() const { return Private::arg(m_message.view(), m_args); }
        //! @}
    };

    /*!
     * Streamable status message, e.g. from Core -> GUI
     */
    class BLACKMISC_EXPORT CStatusMessage :
        public CValueObject<CStatusMessage>,
        public CMessageBase<CStatusMessage>,
        public ITimestampBased,
        public IOrderable
    {
    public:
        //! \copydoc BlackMisc::StatusSeverity
        //! @{
        using StatusSeverity = BlackMisc::StatusSeverity;
        constexpr static auto SeverityDebug   = BlackMisc::SeverityDebug;
        constexpr static auto SeverityInfo    = BlackMisc::SeverityInfo;
        constexpr static auto SeverityWarning = BlackMisc::SeverityWarning;
        constexpr static auto SeverityError   = BlackMisc::SeverityError;
        //! @}

        //! Properties by index
        enum ColumnIndex
        {
            IndexCategoriesAsString = CPropertyIndex::GlobalIndexCStatusMessage,
            IndexCategoriesHumanReadableAsString,
            IndexCategoryHumanReadableOrTechnicalAsString,
            IndexSeverity,
            IndexSeverityAsString,
            IndexSeverityAsIcon,
            IndexMessage,
            IndexMessageNoLineBreaks,
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
        //! @{
        template <size_t N>
        CStatusMessage(const char16_t (&message)[N]) : CStatusMessage(QStringView(message)) {}
        CStatusMessage(const QString &message);
        template <size_t N>
        CStatusMessage(StatusSeverity severity, const char16_t (&message)[N]) : CStatusMessage(severity, QStringView(message)) {}
        CStatusMessage(StatusSeverity severity, const QString &message);
        //! @}

        //! Constructor, also a validation messsage can be directly created
        //! @{
        template <size_t N>
        CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const char16_t (&message)[N], bool validation = false) : CStatusMessage(categories, severity, QStringView(message), validation) {}
        CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const QString &message, bool validation = false);
        //! @}

        //! Deleted constructor, to prevent inefficient construction from byte string literal.
        //! Explicit so as to avoid ambiguities with functions overloaded on QString and CStatusMessage.
        //! @{
        explicit CStatusMessage(const char *message) = delete;
        explicit CStatusMessage(StatusSeverity severity, const char *message) = delete;
        explicit CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const char *message, bool validation = false) = delete;
        //! @}

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

        //! Message without line breaks
        QString getMessageNoLineBreaks() const;

        //! Prepend message
        void prependMessage(const QString &msg);

        //! Append message
        void appendMessage(const QString &msg);

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
        void addValidationCategory() { this->addCategory(CLogCategories::validation()); }

        //! Add categories, avoids duplicates
        void addCategories(const CLogCategoryList &categories) { this->addIfNotExisting(categories); }

        //! Reset category
        void setCategory(const CLogCategory &category) { this->m_categories = CLogCategoryList { category }; }

        //! Reset categories
        void setCategories(const CLogCategoryList &categories) { this->m_categories = categories; }

        //! Representing icon
        CIcons::IconIndex toIcon() const { return convertToIcon(*this).getIndex(); }

        //! Severity as string
        const QString &getSeverityAsString() const;

        //! Severity as icon
        const CIcon &getSeverityAsIcon() const;

        //! Severity as string
        static const QString &severityToString(StatusSeverity severity);

        //! Severity set as string
        static QString severitiesToString(const QSet<StatusSeverity> &severities);

        //! Severity as string, if not possible to convert \sa CSeverityInfo
        static StatusSeverity stringToSeverity(const QString &severity);

        //! Severities as strings
        static const QStringList &allSeverityStrings();

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CStatusMessage &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To HTML
        QString toHtml(bool withIcon, bool withColors) const;

        //! Representing icon
        static const CIcon &convertToIcon(const CStatusMessage &statusMessage);

        //! Representing icon
        static const CIcon &convertToIcon(CStatusMessage::StatusSeverity severity);

        //! Representing icon
        static const QString &convertToIconResource(CStatusMessage::StatusSeverity severity);

        //! Object from JSON
        static CStatusMessage fromDatabaseJson(const QJsonObject &json);

        //! Object from JSON exception message
        static CStatusMessage fromJsonException(const CJsonException &ex, const CLogCategoryList &categories, const QString &prefix);

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

    private:
        CStatusMessage(QStringView message);
        CStatusMessage(StatusSeverity severity, QStringView message);
        CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, QStringView message, bool validation);

        mutable QVector<quintptr> m_handledByObjects;
        mutable QReadWriteLock    m_lock;  //!< lock (because of mutable member)

        //! \fixme KB 2019-01 order and timestamp "disabled" for Ref T184 token bucket. Would it be better to enable those and use a special comparison function for that (e.g. "equalMessageAndSeverity")?
        BLACK_METACLASS(
            CStatusMessage,
            BLACK_METAMEMBER(categories),
            BLACK_METAMEMBER(severity),
            BLACK_METAMEMBER(message),
            BLACK_METAMEMBER(args),
            BLACK_METAMEMBER(order, 0, DisabledForHashing | DisabledForComparison),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForHashing | DisabledForComparison)
        );
    };

    // CContainerBase methods implemented out-of-line to avoid circular include
    template <class Derived>
    CStatusMessage CContainerBase<Derived>::convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromJson(json);
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, categories, prefix);
        }
        return {};
    }

    //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
    template <class Derived>
    CStatusMessage CContainerBase<Derived>::convertFromJsonNoThrow(const QString &jsonString, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromJson(jsonString);
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, categories, prefix);
        }
        return {};
    }
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CStrongStringView)
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage::StatusSeverity)

#endif // guard
