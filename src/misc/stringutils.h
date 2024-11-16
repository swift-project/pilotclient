// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_STRINGUTILS_H
#define SWIFT_MISC_STRINGUTILS_H

#include <algorithm>
#include <atomic>
#include <iosfwd>
#include <string>

#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFlags>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QSet>
#include <QString>
#include <QStringRef>
#include <QStringView>
#include <QTextStream>
#include <QtGlobal>

#include "misc/range.h"
#include "misc/swiftmiscexport.h"
#include "misc/typetraits.h"

template <class T1, class T2>
class QMap;

namespace swift::misc
{
    //! Return a string with characters removed that match the given predicate.
    template <class F>
    QString removeChars(const QString &s, F predicate)
    {
        QString result;
        std::copy_if(s.begin(), s.end(), std::back_inserter(result), [=](auto c) { return !predicate(c); });
        return result;
    }

    //! Remove if in string
    inline QString removeIfInString(const QString &string, const QString &inString)
    {
        return removeChars(string.simplified(), [&](QChar c) { return inString.contains(c); });
    }

    //! Remove if NOT in string
    inline QString removeIfNotInString(const QString &string, const QString &inString)
    {
        return removeChars(string.simplified(), [&](QChar c) { return !inString.contains(c); });
    }

    //! Remove line breaks and tabs
    inline QString removeLineBreakAndTab(const QString &s)
    {
        return removeChars(s, [](QChar c) { return c == '\n' || c == '\t'; });
    }

    //! Remove the typical separators such as "-", " "
    SWIFT_MISC_EXPORT QString removeDateTimeSeparators(const QString &s);

    //! True if any character in the string matches the given predicate.
    template <class F>
    bool containsChar(const QString &s, F predicate)
    {
        return std::any_of(s.begin(), s.end(), predicate);
    }

    //! Contains a line break or tab
    inline bool containsLineBreakOrTab(const QString &s)
    {
        return containsChar(s, [](QChar c) { return c == '\n' || c == '\t'; });
    }

    //! Index of first character in the string matching the given predicate, or -1 if not found.
    template <class F>
    int indexOfChar(const QString &s, F predicate)
    {
        auto it = std::find_if(s.begin(), s.end(), predicate);
        if (it == s.end()) { return -1; }
        return static_cast<int>(std::distance(s.begin(), it));
    }

    //! nth index of ch
    SWIFT_MISC_EXPORT int nthIndexOf(const QString &string, QChar ch, int nth = 1, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

    //! Split a string into multiple strings, using a predicate function to identify the split points.
    //! \warning The returned refs are only valid during the lifetime of the original string.
    template <class F>
    QList<QStringRef> splitStringRefs(const QString &s, F predicate)
    {
        QList<QStringRef> result;
        auto notPredicate = [=](auto c) { return !predicate(c); };
        auto begin = s.begin();
        while (true)
        {
            begin = std::find_if(begin, s.end(), notPredicate);
            if (begin == s.end()) { return result; }
            auto end = std::find_if(begin, s.end(), predicate);
            result.push_back(QStringRef(&s, std::distance(s.begin(), begin), std::distance(begin, end)));
            begin = end;
        }
    }

    //! Split a string into multiple lines. Blank lines are skipped.
    //! \warning The returned refs are only valid during the lifetime of the original string.
    SWIFT_MISC_EXPORT QList<QStringRef> splitLinesRefs(const QString &s);

    //! It would be risky to call splitStringRefs with an rvalue, so forbid it.
    template <class F>
    void splitStringRefs(const QString &&, F) = delete;

    //! It would be risky to call splitLinesRefs with an rvalue, so forbid it.
    void splitLinesRefs(const QString &&) = delete;

    //! Split a string into multiple strings, using a predicate function to identify the split points.
    template <class F>
    QStringList splitString(const QString &s, F predicate)
    {
        return makeRange(splitStringRefs(s, predicate)).transform([](QStringRef sr) { return sr.toString(); });
    }

    //! Split a string into multiple lines. Blank lines are skipped.
    SWIFT_MISC_EXPORT QStringList splitLines(const QString &s);

    //! Extended percent encoding supporting UTF-16
    SWIFT_MISC_EXPORT QByteArray utfToPercentEncoding(const QString &s, const QByteArray &allow = {}, char percent = '%');

    //! Reverse utfFromPercentEncoding
    SWIFT_MISC_EXPORT QString utfFromPercentEncoding(const QByteArray &ba, char percent = '%');

    //! A map converted to string
    template <class K, class V>
    QString qmapToString(const QMap<K, V> &map)
    {
        QString s;
        static const QString kv("%1: %2 ");
        QMapIterator<K, V> i(map);
        while (i.hasNext())
        {
            i.next();
            s.append(
                kv.arg(i.key()).arg(i.value()));
        }
        return s.trimmed();
    }

    //! Obtain ini file like values, e.g. foo=bar
    SWIFT_MISC_EXPORT QMap<QString, QString> parseIniValues(const QString &data);

    //! Is 0-9 char, isDigit allows a bunch of more characters
    inline bool is09(const QChar &c) { return c >= u'0' && c <= u'9'; }

    //! Is 0-9, or ","/";"
    inline bool is09OrSeparator(const QChar &c) { return is09(c) || c == u',' || c == '.'; }

    //! Safe "at" function, returns empty string if index does not exists
    inline const QString &safeAt(const QStringList &stringList, int index)
    {
        if (stringList.size() > index) { return stringList.at(index); }
        static const QString empty;
        return empty;
    }

    //! String with digits only
    inline bool isDigitsOnlyString(const QString &testString)
    {
        return !containsChar(testString, [](QChar c) { return !c.isDigit(); });
    }

    //! String with 0-9 only
    inline bool is09OnlyString(const QString &testString)
    {
        return !containsChar(testString, [](QChar c) { return !is09(c); });
    }

    //! String with 0-9/separator only
    inline bool is09OrSeparatorOnlyString(const QString &testString)
    {
        return !containsChar(testString, [](QChar c) { return !is09OrSeparator(c); });
    }

    //! String only with digits
    inline QString digitOnlyString(const QString &string)
    {
        return removeChars(string, [](QChar c) { return !c.isDigit(); });
    }

    //! String only with 0-9
    inline QString char09OnlyString(const QString &string)
    {
        return removeChars(string, [](QChar c) { return !is09(c); });
    }

    //! String only with 0-9, or separator
    inline QString char09OrSeparatorOnlyString(const QString &string)
    {
        return removeChars(string, [](QChar c) { return !is09OrSeparator(c); });
    }

    //! String only with ASCII values
    inline QString asciiOnlyString(const QString &string)
    {
        return removeChars(string.simplified(), [](QChar c) { return c.unicode() > 127; });
    }

    //! Return string in apostrophes
    SWIFT_MISC_EXPORT QString inApostrophes(const QString &in, bool ignoreEmpty = false);

    //! Return string in quotes
    SWIFT_MISC_EXPORT QString inQuotes(const QString &in, bool ignoreEmpty = false);

    //! Has balanced quotes
    SWIFT_MISC_EXPORT bool hasBalancedQuotes(const QString &in, char quote = '"');

    //! Remove surrounding apostrophes 'foo' -> foo
    SWIFT_MISC_EXPORT QString removeSurroundingApostrophes(const QString &in);

    //! Remove surrounding quotes "foo" -> foo
    SWIFT_MISC_EXPORT QString removeSurroundingQuotes(const QString &in);

    //! Bool to on/off
    SWIFT_MISC_EXPORT const QString &boolToOnOff(bool v);

    //! Bool to yes/no
    SWIFT_MISC_EXPORT const QString &boolToYesNo(bool v);

    //! Bool to true/false
    SWIFT_MISC_EXPORT const QString &boolToTrueFalse(bool v);

    //! Bool to enabled/disabled
    SWIFT_MISC_EXPORT const QString &boolToEnabledDisabled(bool v);

    //! Bool isNull to null/no null
    SWIFT_MISC_EXPORT const QString &boolToNullNotNull(bool isNull);

    //! Convert string to bool
    SWIFT_MISC_EXPORT bool stringToBool(const QString &boolString);

    //! Fuzzy compare for short strings (like ICAO designators)
    //! \return int 0..100 (100 is perfect match)
    SWIFT_MISC_EXPORT int fuzzyShortStringComparision(const QString &str1, const QString &str2, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    //! Int to hex value
    SWIFT_MISC_EXPORT QString intToHex(int value, int digits = 2);

    //! Replace dot '.' by locale decimal point
    SWIFT_MISC_EXPORT QString dotToLocaleDecimalPoint(QString &input);

    //! Replace dot '.' by locale decimal point
    SWIFT_MISC_EXPORT QString dotToLocaleDecimalPoint(const QString &input);

    //! Strip a designator from a combined string
    SWIFT_MISC_EXPORT QString stripDesignatorFromCompleterString(const QString &candidate);

    //! Strip a designator from a combined string
    SWIFT_MISC_EXPORT QStringList textCodecNames(bool simpleNames, bool mibNames);

    //! Remove accents / diacritic marks from a string
    SWIFT_MISC_EXPORT QString simplifyAccents(const QString &candidate);

    //! Remove accents / diacritic marks from a string by doing a Unicode decomposition and removing mark characters
    SWIFT_MISC_EXPORT QString simplifyByDecomposition(const QString &candidate);

    //! Case insensitive string compare
    SWIFT_MISC_EXPORT bool caseInsensitiveStringCompare(const QString &c1, const QString &c2);

    //! String compare
    SWIFT_MISC_EXPORT bool stringCompare(const QString &c1, const QString &c2, Qt::CaseSensitivity cs);

    //! Get a simplified upper case name for searching by removing all characters except A-Z
    SWIFT_MISC_EXPORT QString simplifyNameForSearch(const QString &name);

    //! Add a question mark at the end if not existing
    SWIFT_MISC_EXPORT QString withQuestionMark(const QString &question);

    //! Same as QDateTime::fromString but QDateTime will be set to UTC
    //! \remark potentially slow, so only to be used when format is unknown
    SWIFT_MISC_EXPORT QDateTime fromStringUtc(const QString &dateTimeString, const QString &format);

    //! Same as QDateTime::fromString but QDateTime will be set to UTC
    //! \remark potentially slow, so only to be used when format is unknown
    SWIFT_MISC_EXPORT QDateTime fromStringUtc(const QString &dateTimeString, Qt::DateFormat format = Qt::TextDate);

    //! Same as QDateTime::fromString but QDateTime will be set to UTC
    //! \remark potentially slow, so only to be used when format is unknown
    SWIFT_MISC_EXPORT QDateTime fromStringUtc(const QString &dateTimeString, const QLocale &locale, QLocale::FormatType format);

    //! Parse multiple date time formats
    //! \remark potentially slow, so only to be used when format is unknown
    //! \remark TZ is UTC
    SWIFT_MISC_EXPORT QDateTime parseMultipleDateTimeFormats(const QString &dateTimeString);

    //! Parse yyyyMMddHHmmsszzz strings optimized
    //! \remark string needs to be cleaned up and containing only numbers
    //! \remark TZ is UTC
    SWIFT_MISC_EXPORT QDateTime parseDateTimeStringOptimized(const QString &dateTimeString);

    //! Convert string to bool
    SWIFT_MISC_EXPORT QString joinStringSet(const QSet<QString> &set, const QString &separator);

    //! Remove comments such as /** **/ or //
    SWIFT_MISC_EXPORT QString removeComments(const QString &in, bool removeSlash, bool removeDoubleSlash);

    //! Default string if string is empty
    SWIFT_MISC_EXPORT const QString &defaultIfEmpty(const QString &candidate, const QString &defaultIfEmpty);

    //! Contains any string of the list?
    SWIFT_MISC_EXPORT bool containsAny(const QString &testString, const QStringList &any, Qt::CaseSensitivity cs);

    //! Parse a fraction like 2/3
    SWIFT_MISC_EXPORT double parseFraction(const QString &fraction, double failDefault = std::numeric_limits<double>::quiet_NaN());

    //! Remove leading 0, trailing 0, " ", and "." from a number
    SWIFT_MISC_EXPORT QString cleanNumber(const QString &number);

    /*!
     * Stringification traits class.
     */
    template <typename T, typename = void>
    struct TString;

    // Stringification traits specializations.
    //! \cond
    template <>
    struct TString<QString>
    {
        static QString toQString(const QString &s) { return s; }
    };
    template <>
    struct TString<QStringRef>
    {
        static QString toQString(const QStringRef &sr) { return sr.toString(); }
    };
    template <>
    struct TString<QStringView>
    {
        static QString toQString(QStringView sv) { return sv.toString(); }
    };
    template <>
    struct TString<QChar>
    {
        static QString toQString(QChar c) { return c; }
    };
    template <>
    struct TString<char>
    {
        static QString toQString(char c) { return QChar(c); }
    };
    template <>
    struct TString<bool>
    {
        static QString toQString(bool n) { return QString::number(n); }
    };
    template <>
    struct TString<int>
    {
        static QString toQString(int n) { return QString::number(n); }
    };
    template <>
    struct TString<uint>
    {
        static QString toQString(uint n) { return QString::number(n); }
    };
    template <>
    struct TString<long>
    {
        static QString toQString(long n) { return QString::number(n); }
    };
    template <>
    struct TString<ulong>
    {
        static QString toQString(ulong n) { return QString::number(n); }
    };
    template <>
    struct TString<qlonglong>
    {
        static QString toQString(qlonglong n) { return QString::number(n); }
    };
    template <>
    struct TString<qulonglong>
    {
        static QString toQString(qulonglong n) { return QString::number(n); }
    };
    template <>
    struct TString<short>
    {
        static QString toQString(short n) { return QString::number(n); }
    };
    template <>
    struct TString<ushort>
    {
        static QString toQString(ushort n) { return QString::number(n); }
    };
    template <>
    struct TString<float>
    {
        static QString toQString(float n) { return QString::number(n); }
    };
    template <>
    struct TString<double>
    {
        static QString toQString(double n) { return QString::number(n); }
    };
    template <typename T>
    struct TString<T, std::enable_if_t<std::is_enum_v<T>>>
    {
        static QString toQString(T e) { return QString::number(e); }
    };
    template <typename T>
    struct TString<T, std::enable_if_t<std::is_convertible_v<T, QString>>>
    {
        static QString toQString(const T &v) { return v; }
    };
    template <typename T>
    struct TString<T, std::enable_if_t<THasToQString<T>::value>>
    {
        static QString toQString(const T &v) { return v.toQString(); }
    };
    template <typename T>
    struct TString<QFlags<T>>
    {
        static QString toQString(QFlags<T> n) { return TString<typename QFlags<T>::Int>::toQString(n); }
    };
    template <typename T>
    struct TString<std::atomic<T>>
    {
        static QString toQString(const std::atomic<T> &n) { return TString<T>::toQString(n); }
    };
    //! \endcond
} // namespace swift::misc

#endif // guard
