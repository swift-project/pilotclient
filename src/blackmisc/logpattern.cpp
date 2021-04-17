/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/logpattern.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/sequence.h"

#include <QHash>
#include <QList>
#include <QStringList>
#include <algorithm>
#include <tuple>

namespace BlackMisc
{
    const QHash<QString, CLogPattern> &CLogPattern::allHumanReadablePatterns()
    {
        static const QHash<QString, CLogPattern> patterns
        {
            { "uncategorized (swift)",       exactMatch(CLogCategories::uncategorized()) },
            { "background task",             exactMatch(CLogCategories::worker()) },
            { "cache",                       exactMatch(CLogCategories::cache()) },
            { "cmd.line handling",           exactMatch(CLogCategories::cmdLine()) },
            { "data inconsistency",          exactMatch(CLogCategories::dataInconsistency()) },
            { "DBus",                        exactMatch(CLogCategories::dbus()) },
            { "downloading data",            exactMatch(CLogCategories::download()) },
            { "driver",                      exactMatch(CLogCategories::driver()) },
            { "flight plan",                 exactMatch(CLogCategories::flightPlan()) },
            { "FSD",                         exactMatch(CLogCategories::fsd()) },
            { "interpolator",                exactMatch(CLogCategories::interpolator()) },
            { "JSON (conversion)",           exactMatch(CLogCategories::json()) },
            { "model cache",                 exactMatch(CLogCategories::modelCache()) },
            { "model GUI",                   exactMatch(CLogCategories::modelGui()) },
            { "model loader",                exactMatch(CLogCategories::modelLoader()) },
            { "model mapping",               exactMatch(CLogCategories::mapping()) },
            { "model matching",              exactMatch(CLogCategories::matching()) },
            { "model set cache",             exactMatch(CLogCategories::modelSetCache()) },
            { "network (flight)",            exactMatch(CLogCategories::network()) },
            { "plugin",                      exactMatch(CLogCategories::plugin()) },
            { "services",                    exactMatch(CLogCategories::services()) },
            { "settings",                    exactMatch(CLogCategories::settings()) },
            { "startup phase",               exactMatch(CLogCategories::startup()) },
            { "swift context slots",         exactMatch(CLogCategories::contextSlot()) },
            { "swift contexts",              exactMatch(CLogCategories::context()) },
            { "swift core",                  exactMatch(CLogCategories::swiftCore()) },
            { "swift data tool",             exactMatch(CLogCategories::swiftDataTool()) },
            { "swift DB webservice related", exactMatch(CLogCategories::swiftDbWebservice()) },
            { "swift GUI",                   exactMatch(CLogCategories::guiComponent()) },
            { "swift pilot client",          exactMatch(CLogCategories::swiftPilotClient()) },
            { "validation",                  exactMatch(CLogCategories::validation()) },
            { "VATSIM specific",             exactMatch(CLogCategories::vatsimSpecific()) },
            { "verification",                exactMatch(CLogCategories::verification()) },
            { "webservice related",          exactMatch(CLogCategories::webservice()) },
            { "wizard",                      exactMatch(CLogCategories::wizard()) },
            { "Qt library",                  startsWith("qt.") },
            { "uncategorized (default)",     exactMatch("default") },
            { "uncategorized (none)",        empty() }
        };
        return patterns;
    }

    const QStringList &CLogPattern::allHumanReadableNames()
    {
        static const QStringList names = allHumanReadablePatterns().keys();
        return names;
    }

    QStringList CLogPattern::humanReadableNamesFrom(const CStatusMessage &message)
    {
        QStringList patternNames;
        for (const QString &name : CLogPattern::allHumanReadableNames())
        {
            if (CLogPattern::fromHumanReadableName(name).match(message)) { patternNames.push_back(name); }
        }
        return patternNames;
    }

    QStringList CLogPattern::humanOrTechnicalCategoriesFrom(const CStatusMessage &message)
    {
        if (message.getCategories().isEmpty()) { return {}; }
        QStringList c(humanReadableNamesFrom(message).join(", "));
        return c.isEmpty() ? message.getCategories().toQStringList() : c;
    }

    const CLogPattern &CLogPattern::fromHumanReadableName(const QString &name)
    {
        static const CLogPattern empty {};
        auto it = allHumanReadablePatterns().find(name);
        return it == allHumanReadablePatterns().end() ? empty : *it;
    }

    CLogPattern::CLogPattern(Strategy strategy, const QSet<QString> &strings)
        : m_strategy(strategy), m_strings(strings)
    {
        static const decltype(m_severities) s
        {
            CStatusMessage::SeverityDebug,
            CStatusMessage::SeverityInfo,
            CStatusMessage::SeverityWarning,
            CStatusMessage::SeverityError
        };
        m_severities = s;
    }

    CLogPattern::CLogPattern() : CLogPattern(Everything, {})
    {}

    CLogPattern CLogPattern::exactMatch(const CLogCategory &category)
    {
        return { ExactMatch, { category.toQString() } };
    }

    CLogPattern CLogPattern::anyOf(const CLogCategoryList &categories)
    {
        if (categories.size() == 0) { return empty(); }
        if (categories.size() == 1) { return exactMatch(categories[0]); }
        const QStringList strList = categories.toQStringList();
        return { AnyOf, QSet<QString>(strList.begin(), strList.end()) };
    }

    CLogPattern CLogPattern::allOf(const CLogCategoryList &categories)
    {
        if (categories.size() == 0) { return {}; }
        if (categories.size() == 1) { return exactMatch(categories[0]); }
        const QStringList strList = categories.toQStringList();
        return { AnyOf, QSet<QString>(strList.begin(), strList.end()) };
    }

    CLogPattern CLogPattern::startsWith(const QString &prefix)
    {
        return { StartsWith, { prefix } };
    }

    CLogPattern CLogPattern::endsWith(const QString &suffix)
    {
        return { EndsWith, { suffix } };
    }

    CLogPattern CLogPattern::contains(const QString &substring)
    {
        return { Contains, { substring } };
    }

    CLogPattern CLogPattern::empty()
    {
        return { Nothing, {} };
    }

    CLogPattern CLogPattern::withSeverity(CStatusMessage::StatusSeverity severity) const
    {
        auto result = *this;
        result.m_severities = { severity };
        return result;
    }

    CLogPattern CLogPattern::withSeverities(const QSet<CStatusMessage::StatusSeverity> &severities) const
    {
        auto result = *this;
        result.m_severities = severities;
        return result;
    }

    CLogPattern CLogPattern::withSeverityAtOrAbove(CStatusMessage::StatusSeverity minimumSeverity) const
    {
        auto result = *this;
        result.m_severities.clear();
        switch (minimumSeverity)
        {
        default:
        case CStatusMessage::SeverityDebug:     result.m_severities.insert(CStatusMessage::SeverityDebug);
        [[fallthrough]];
        case CStatusMessage::SeverityInfo:      result.m_severities.insert(CStatusMessage::SeverityInfo);
        [[fallthrough]];
        case CStatusMessage::SeverityWarning:   result.m_severities.insert(CStatusMessage::SeverityWarning);
        [[fallthrough]];
        case CStatusMessage::SeverityError:     result.m_severities.insert(CStatusMessage::SeverityError);
        }
        return result;
    }

    bool CLogPattern::checkInvariants() const
    {
        switch (m_strategy)
        {
        case Everything:    return m_strings.isEmpty();
        case ExactMatch:    return m_strings.size() == 1;
        case AnyOf:         return m_strings.size() > 1;
        case AllOf:         return m_strings.size() > 1;
        case StartsWith:    return m_strings.size() == 1;
        case EndsWith:      return m_strings.size() == 1;
        case Contains:      return m_strings.size() == 1;
        case Nothing:       return m_strings.isEmpty();
        default:            return false;
        }
    }

    bool CLogPattern::match(const CStatusMessage &message) const
    {
        if (! checkInvariants())
        {
            Q_ASSERT(false);
            return true;
        }

        if (! m_severities.contains(message.getSeverity()))
        {
            return false;
        }

        switch (m_strategy)
        {
        default:
        case Everything:    return true;
        case ExactMatch:    return message.getCategories().contains(getString());
        case AnyOf:         return std::any_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return message.getCategories().contains(s); });
        case AllOf:         return std::all_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return message.getCategories().contains(s); });
        case StartsWith:    return message.getCategories().containsBy([this](const CLogCategory & cat) { return cat.startsWith(getPrefix()); });
        case EndsWith:      return message.getCategories().containsBy([this](const CLogCategory & cat) { return cat.endsWith(getSuffix()); });
        case Contains:      return message.getCategories().containsBy([this](const CLogCategory & cat) { return cat.contains(getSubstring()); });
        case Nothing:       return message.getCategories().isEmpty();
        }
    }

    bool CLogPattern::isProperSubsetOf(const CLogPattern &other) const
    {
        if (!(checkInvariants() && other.checkInvariants()))
        {
            Q_ASSERT(false);
            return false;
        }

        // For this function to return true, the severities matched by this pattern must be a subset of the severities
        // matched by the other, and the categories matched by this pattern must be a subset of the categories matched
        // by the other, and at least one of these two subset relations must be a proper subset relation.

        if (! other.m_severities.contains(m_severities))
        {
            // Severities are not a subset
            return false;
        }
        if (m_strategy == other.m_strategy && m_strings == other.m_strings)
        {
            // Severities are a subset, and categories are an improper subset, so it all depends
            // on whether the subset relation of the severities is proper or improper
            return m_severities.size() < other.m_severities.size();
        }

        // If we got this far then the severity set is a (proper or improper) subset of the other severity set,
        // and the question of whether this pattern is a proper subset of the other pattern depends upon whether the
        // set of categories matched by this pattern is a proper subset of the set of categories matched by the other.
        // The matrix below is a guide to the implementation that follows.
        //
        //  Matrix of "categories matched by X is proper subset of categories matched by Y" for two strategies X and Y
        //  0 means always false
        //  1 means always true
        //  ? means it depends on a particular relation between their string sets
        //
        //    Y Ev EM An Al SW EW Co No
        //  X
        //  Ev   0  0  0  0  0  0  0  0   (Everything)
        //  EM   1  0  ?  0  ?  ?  ?  0   (ExactMatch)
        //  An   1  0  ?  0  ?  ?  ?  0   (AnyOf)
        //  Al   1  ?  ?  ?  ?  ?  ?  0   (AllOf)
        //  SW   1  0  0  0  ?  0  ?  0   (StartsWith)
        //  EW   1  0  0  0  0  ?  ?  0   (EndsWith)
        //  Co   1  0  0  0  0  0  ?  0   (Contains)
        //  No   1  0  0  0  0  0  0  0   (Nothing)

        if (m_strategy != Everything && other.m_strategy == Everything)
        {
            return true;
        }
        switch (m_strategy)
        {
        case ExactMatch:
            switch (other.m_strategy)
            {
            case AnyOf:         return other.m_strings.contains(getString());
            case StartsWith:    return getString().startsWith(other.getPrefix());
            case EndsWith:      return getString().endsWith(other.getSuffix());
            case Contains:      return getString().contains(other.getSubstring());
            default:            ;
            }
            break;
        case AnyOf:
            switch (other.m_strategy)
            {
            case AnyOf:         return other.m_strings.contains(m_strings) && other.m_strings.size() > m_strings.size();
            case StartsWith:    return std::all_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.startsWith(other.getPrefix()); });
            case EndsWith:      return std::all_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.endsWith(other.getSuffix()); });
            case Contains:      return std::all_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.contains(other.getSubstring()); });
            default:            ;
            }
            break;
        case AllOf:
            switch (other.m_strategy)
            {
            case ExactMatch:    return m_strings.contains(other.getString());
            case AnyOf:         return !(m_strings & other.m_strings).isEmpty();
            case AllOf:         return m_strings.contains(other.m_strings) && m_strings.size() > other.m_strings.size();
            case StartsWith:    return std::any_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.startsWith(other.getPrefix()); });
            case EndsWith:      return std::any_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.endsWith(other.getSuffix()); });
            case Contains:      return std::any_of(m_strings.begin(), m_strings.end(), [ & ](const QString & s) { return s.contains(other.getSubstring()); });
            default:            ;
            }
            break;
        case StartsWith:
            switch (other.m_strategy)
            {
            case StartsWith:    return getPrefix().startsWith(other.getPrefix()) && getPrefix().size() > other.getPrefix().size();
            case Contains:      return getPrefix().contains(other.getSubstring());
            default:            ;
            }
            break;
        case EndsWith:
            switch (other.m_strategy)
            {
            case EndsWith:      return getSuffix().endsWith(other.getSuffix()) && getSuffix().size() > other.getSuffix().size();
            case Contains:      return getSuffix().contains(other.getSubstring());
            default:            ;
            }
            break;
        case Contains:
            switch (other.m_strategy)
            {
            case Contains:      return getSubstring().contains(other.getSubstring()) && getSubstring().size() > other.getSubstring().size();
            default:            ;
            }
            break;
        default:;
        }
        return false;
    }

    QString CLogPattern::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        QString strategy;
        QString categories = m_strings.values().join("|"); // clazy:exclude=container-anti-pattern
        switch (m_strategy)
        {
        case Everything: strategy = "all"; break;
        case ExactMatch: strategy = "exact match:" + categories; break;
        case AnyOf: strategy = "any of:" + categories; break;
        case AllOf: strategy = "all of:" + categories; break;
        case StartsWith: strategy = "starts with:" + categories; break;
        case EndsWith: strategy = "ends with:" + categories; break;
        case Contains: strategy = "contains:" + categories; break;
        case Nothing: strategy = "none"; break;
        default: strategy = "<invalid>"; break;
        }
        return "{" + CStatusMessage::severitiesToString(m_severities) + "," + strategy + "}";
    }

    void CLogPattern::marshallToDbus(QDBusArgument &argument) const
    {
        // a bug in QtDBus prevents us from marshalling m_severities as a list
        quint8 severities = 0;
        for (auto s : m_severities) { severities |= (1 << static_cast<int>(s)); }

        argument << severities << m_strategy << m_strings.values();
    }

    void CLogPattern::unmarshallFromDbus(const QDBusArgument &argument)
    {
        quint8 severities;
        QStringList strings;
        argument >> severities >> m_strategy >> strings;
        m_strings = QSet<QString>(strings.begin(), strings.end());

        m_severities.clear();
        for (int s : { 0, 1, 2, 3 })
        {
            if (severities & (1 << s)) { m_severities.insert(static_cast<CStatusMessage::StatusSeverity>(s)); }
        }
    }

    void CLogPattern::marshalToDataStream(QDataStream &stream) const
    {
        quint8 severities = 0;
        for (auto s : m_severities) { severities |= (1 << static_cast<int>(s)); }

        stream << severities << m_strategy << m_strings.values();
    }

    void CLogPattern::unmarshalFromDataStream(QDataStream &stream)
    {
        quint8 severities;
        QStringList strings;
        stream >> severities >> m_strategy >> strings;
        m_strings = QSet<QString>(strings.begin(), strings.end());

        m_severities.clear();
        for (int s : { 0, 1, 2, 3 })
        {
            if (severities & (1 << s)) { m_severities.insert(static_cast<CStatusMessage::StatusSeverity>(s)); }
        }
    }
}
