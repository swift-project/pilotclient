/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_LOGPATTERN_H
#define BLACKMISC_LOGPATTERN_H

//! \file

#include "blackmiscexport.h"
#include "statusmessage.h"
#include <QExplicitlySharedDataPointer>

namespace BlackMisc
{
    class CLogCategory;
    class CLogCategoryList;

    class CLogPattern;

    //! \private
    template <> struct CValueObjectPolicy<CLogPattern> : public CValueObjectPolicy<>
    {
        using LessThan = Policy::LessThan::None;
        using Compare = Policy::Compare::None;
        using Hash = Policy::Hash::None;
        using DBus = Policy::DBus::Own;
        using Json = Policy::Json::None;
    };

    /*!
     * Value class for matching log messages based on their categories.
     */
    class BLACKMISC_EXPORT CLogPattern : public CValueObject<CLogPattern>
    {
    public:
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

        //! Returns a CLogPattern which will match the same messages, but only with a severity at or above the given severity.
        CLogPattern withSeverityAtOrAbove(CStatusMessage::StatusSeverity minimumSeverity) const;

        //! Returns true if the given message matches this pattern.
        bool match(const CStatusMessage &message) const;

        //! Returns true if this pattern is a proper subset of the other pattern.
        //! \see     https://en.wikipedia.org/wiki/Proper_subset
        //! \details Pattern A is a proper subset of pattern B iff pattern B would match every category which pattern A matches,
        //!          plus at least one other category. This induces a partial ordering which can be used as the comparator in a
        //!          topological sorting algorithm, to sort patterns by their generality.
        bool isProperSubsetOf(const CLogPattern &other) const;

    protected:
        //! \copydoc CValueObject::convertToQString()
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::marshallToDbus()
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::marshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

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

        BLACK_ENABLE_TUPLE_CONVERSION(CLogPattern)
        QSet<CStatusMessage::StatusSeverity> m_severities;
        Strategy m_strategy;
        QSet<QString> m_strings;

        const QString &getString() const { Q_ASSERT(m_strategy == ExactMatch && m_strings.size() == 1); return *m_strings.begin(); }
        const QString &getPrefix() const { Q_ASSERT(m_strategy == StartsWith && m_strings.size() == 1); return *m_strings.begin(); }
        const QString &getSuffix() const { Q_ASSERT(m_strategy == EndsWith && m_strings.size() == 1); return *m_strings.begin(); }
        const QString &getSubstring() const { Q_ASSERT(m_strategy == Contains && m_strings.size() == 1); return *m_strings.begin(); }
    };
}

Q_DECLARE_METATYPE(BlackMisc::CLogPattern)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CLogPattern, (
    attr(o.m_severities, flags<DisabledForHashing>()),
    attr(o.m_strategy),
    attr(o.m_strings, flags<DisabledForHashing>())
))

#endif
