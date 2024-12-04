// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOGPATTERN_H
#define SWIFT_MISC_LOGPATTERN_H

#include <QDBusArgument>
#include <QMetaType>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include "misc/metaclass.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinicon.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

template <class Key, class T>
class QHash;

namespace swift::misc
{
    class CLogCategory;
    class CLogCategoryList;

    /*!
     * Value class for matching log messages based on their categories.
     */
    class SWIFT_MISC_EXPORT CLogPattern :
        public mixin::MetaType<CLogPattern>,
        public mixin::HashByMetaClass<CLogPattern>,
        public mixin::EqualsByMetaClass<CLogPattern>,
        public mixin::DBusOperators<CLogPattern>,
        public mixin::DataStreamOperators<CLogPattern>,
        public mixin::Index<CLogPattern>,
        public mixin::String<CLogPattern>,
        public mixin::Icon<CLogPattern>
    {
    public:
        //! Get a list of human-readable names of predefined log patterns.
        static const QStringList &allHumanReadableNames();

        //! Return a predefined CLogPattern corresponding to the given human-readable name.
        static const CLogPattern &fromHumanReadableName(const QString &name);

        //! Human readable categories of message.
        static QStringList humanReadableNamesFrom(const CStatusMessage &message);

        //! Human or machine readable categories of message.
        static QStringList humanOrTechnicalCategoriesFrom(const CStatusMessage &message);

        //! Default constructed CLogPattern will match any message.
        CLogPattern();

        //! Returns a CLogPattern which will match any message with the given category.
        static CLogPattern exactMatch(const CLogCategory &category);

        //! Returns a CLogPattern which will match any message with any of the given categories.
        static CLogPattern anyOf(const CLogCategoryList &categories);

        //! Returns a CLogPattern which will match any message with all of the given categories.
        static CLogPattern allOf(const CLogCategoryList &categories);

        //! Returns a CLogPattern which will match any message with a category which starts with the given prefix.
        static CLogPattern startsWith(const QString &prefix);

        //! Returns a CLogPattern which will match any message with a category which ends with the given suffix.
        static CLogPattern endsWith(const QString &suffix);

        //! Returns a CLogPattern which will match any message with a category which contains the given substring.
        static CLogPattern contains(const QString &substring);

        //! Returns a CLogPattern which will match any message without a category.
        static CLogPattern empty();

        //! Returns a CLogPattern which will match the same messages as this one, but only with a given severity.
        CLogPattern withSeverity(CStatusMessage::StatusSeverity severity) const;

        //! Returns a CLogPattern which will match the same messages as this one, but only with some given severities.
        CLogPattern withSeverities(const QSet<CStatusMessage::StatusSeverity> &severities) const;

        //! Returns a CLogPattern which will match the same messages, but only with a severity at or above the given
        //! severity.
        CLogPattern withSeverityAtOrAbove(CStatusMessage::StatusSeverity minimumSeverity) const;

        //! Returns true if the given message matches this pattern.
        bool match(const CStatusMessage &message) const;

        //! This class acts as a SharedState filter when stored in a CVariant.
        bool matches(const CVariant &message) const { return match(message.to<CStatusMessage>()); }

        //! Technical category names matched by this pattern.
        QSet<QString> getCategoryStrings() const { return m_strings; }

        //! Returns true if this pattern is a proper subset of the other pattern.
        //! \see     https://en.wikipedia.org/wiki/Proper_subset
        //! \details Pattern A is a proper subset of pattern B iff pattern B would match every category which pattern A
        //! matches,
        //!          plus at least one other category. This induces a partial ordering which can be used as the
        //!          comparator in a topological sorting algorithm, to sort patterns by their generality.
        bool isProperSubsetOf(const CLogPattern &other) const;

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::marshallToDbus()
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus()
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);

    private:
        bool checkInvariants() const;

        enum Strategy
        {
            Everything = 0,
            ExactMatch,
            AnyOf,
            AllOf,
            StartsWith,
            EndsWith,
            Contains,
            Nothing
        };

        CLogPattern(Strategy strategy, const QSet<QString> &strings);

        QSet<CStatusMessage::StatusSeverity> m_severities;
        Strategy m_strategy;
        QSet<QString> m_strings;

        const QString &getString() const
        {
            Q_ASSERT(m_strategy == ExactMatch && m_strings.size() == 1);
            return *m_strings.begin();
        }
        const QString &getPrefix() const
        {
            Q_ASSERT(m_strategy == StartsWith && m_strings.size() == 1);
            return *m_strings.begin();
        }
        const QString &getSuffix() const
        {
            Q_ASSERT(m_strategy == EndsWith && m_strings.size() == 1);
            return *m_strings.begin();
        }
        const QString &getSubstring() const
        {
            Q_ASSERT(m_strategy == Contains && m_strings.size() == 1);
            return *m_strings.begin();
        }

        static const QHash<QString, CLogPattern> &allHumanReadablePatterns();

        SWIFT_METACLASS(
            CLogPattern,
            SWIFT_METAMEMBER(severities),
            SWIFT_METAMEMBER(strategy),
            SWIFT_METAMEMBER(strings));
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CLogPattern)

#endif // SWIFT_MISC_LOGPATTERN_H
