// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "blackmisc/stringutils.h"
#include <QChar>
#include <QTextCodec>
#include <QStringBuilder>
#include <QRegularExpression>

namespace BlackMisc
{
    QString removeDateTimeSeparators(const QString &s)
    {
        return removeChars(s, [](QChar c) {
            return c == u' ' || c == u':' || c == u'_' || c == u'-' || c == u'.';
        });
    }

    QList<QStringRef> splitLinesRefs(const QString &s)
    {
        return splitStringRefs(s, [](QChar c) { return c == '\n' || c == '\r'; });
    }

    QStringList splitLines(const QString &s)
    {
        return splitString(s, [](QChar c) { return c == '\n' || c == '\r'; });
    }

    QByteArray utfToPercentEncoding(const QString &s, const QByteArray &allow, char percent)
    {
        QByteArray result;
        for (const QChar &c : s)
        {
            if (const char latin = c.toLatin1())
            {
                if ((latin >= 'a' && latin <= 'z') || (latin >= 'A' && latin <= 'Z') || (latin >= '0' && latin <= '9') || allow.contains(latin))
                {
                    result += c.toLatin1();
                }
                else
                {
                    result += percent;
                    if (latin < 0x10) { result += '0'; }
                    result += QByteArray::number(static_cast<int>(latin), 16);
                }
            }
            else
            {
                result += percent;
                result += 'x';
                const ushort unicode = c.unicode();
                if (unicode < 0x0010) { result += '0'; }
                if (unicode < 0x0100) { result += '0'; }
                if (unicode < 0x1000) { result += '0'; }
                result += QByteArray::number(unicode, 16);
            }
        }
        return result;
    }

    QString utfFromPercentEncoding(const QByteArray &ba, char percent)
    {
        QString result;
        for (int i = 0; i < ba.size(); ++i)
        {
            if (ba[i] == percent)
            {
                ++i;
                Q_ASSERT(i < ba.size());
                if (ba[i] == 'x')
                {
                    ++i;
                    Q_ASSERT(i < ba.size());
                    result += QChar(ba.mid(i, 4).toInt(nullptr, 16));
                    i += 3;
                }
                else
                {
                    result += static_cast<char>(ba.mid(i, 2).toInt(nullptr, 16));
                    ++i;
                }
            }
            else
            {
                result += ba[i];
            }
        }
        return result;
    }

    const QString &boolToOnOff(bool v)
    {
        static const QString on("on");
        static const QString off("off");
        return v ? on : off;
    }

    const QString &boolToYesNo(bool v)
    {
        static const QString yes("yes");
        static const QString no("no");
        return v ? yes : no;
    }

    const QString &boolToTrueFalse(bool v)
    {
        static const QString t("true");
        static const QString f("false");
        return v ? t : f;
    }

    const QString &boolToEnabledDisabled(bool v)
    {
        static const QString e("enabled");
        static const QString d("disabled");
        return v ? e : d;
    }

    const QString &boolToNullNotNull(bool isNull)
    {
        static const QString n("null");
        static const QString nn("not null");
        return isNull ? n : nn;
    }

    bool stringToBool(const QString &string)
    {
        QString s(string.trimmed().toLower());
        if (s.isEmpty()) { return false; }

        // 1 char values
        const QChar c = s.at(0);
        if (c == '1' || c == 't' || c == 'y' || c == 'x') { return true; }
        if (c == '0' || c == 'f' || c == 'n' || c == '_') { return false; }

        if (c == 'e') { return true; } // enabled
        if (c == 'd') { return false; } // disabled

        // full words
        if (s == "on") { return true; }
        return false;
    }

    int fuzzyShortStringComparision(const QString &str1, const QString &str2, Qt::CaseSensitivity cs)
    {
        // same
        if (cs == Qt::CaseInsensitive)
        {
            if (caseInsensitiveStringCompare(str1, str2)) { return 100; }
        }
        else if (str1 == str2) { return 100; }

        // one string is empty
        if (str1.isEmpty() || str2.isEmpty()) { return 0; }

        // make sure aStr is not shorter
        const QString aStr = str1.length() >= str2.length() ? str1 : str2;
        const QString bStr = str1.length() >= str2.length() ? str2 : str1;

        // starts/ends with
        const double s1 = aStr.length();
        const double s2 = bStr.length();
        if (aStr.endsWith(bStr, cs)) { return qRound(s1 / s2 * 100); }
        if (aStr.startsWith(bStr, cs)) { return qRound(s1 / s2 * 100); }

        // contains
        if (aStr.contains(bStr, cs)) { return qRound(s1 / s2 * 100); }

        // char by char
        double points = 0;
        for (int p = 0; p < aStr.length(); p++)
        {
            if (p < bStr.length() && aStr[p] == bStr[p])
            {
                points += 1.0;
                continue;
            }

            // char after
            const int after = p + 1;
            if (after < bStr.length() && aStr[p] == bStr[after])
            {
                points += 0.5;
                continue;
            }

            // char before
            const int before = p - 1;
            if (before >= 0 && before < bStr.length() && aStr[p] == bStr[before])
            {
                points += 0.5;
                continue;
            }
        }
        return qRound(points / s1 * 100);
    }

    QString intToHex(int value, int digits)
    {
        QString hex(QString::number(value, 16).toUpper());
        int l = hex.length();
        if (l >= digits) { return hex.right(digits); }
        int d = digits - l;
        return QString(d, '0') + hex;
    }

    QString stripDesignatorFromCompleterString(const QString &candidate)
    {
        const QString s(candidate.trimmed().toUpper());
        if (s.isEmpty()) { return QString(); }
        return s.contains(' ') ? s.left(s.indexOf(' ')) : s;
    }

    QStringList simpleTextCodecNamesImpl()
    {
        QStringList codecs;
        for (const QByteArray &ba : QTextCodec::availableCodecs())
        {
            const QString c(QString::fromLocal8Bit(ba));
            codecs << c;
        }
        return codecs;
    }

    QStringList mibTextCodecNamesImpl()
    {
        QStringList codecs;
        for (int mib : QTextCodec::availableMibs())
        {
            const QByteArray ba(QTextCodec::codecForMib(mib)->name());
            const QString c(QString::fromLocal8Bit(ba));
            codecs << c;
        }
        return codecs;
    }

    QStringList textCodecNames(bool simpleNames, bool mibNames)
    {
        static const QStringList simple(simpleTextCodecNamesImpl());
        static const QStringList mib(mibTextCodecNamesImpl());
        if (simpleNames && mibNames)
        {
            QStringList s(simple);
            s.append(mib);
            return s;
        }
        if (simpleNames) { return simple; }
        if (mibNames) { return mib; }
        return QStringList();
    }

    // http://www.codegur.online/14009522/how-to-remove-accents-diacritic-marks-from-a-string-in-qt
    // https://stackoverflow.com/questions/14009522/how-to-remove-accents-diacritic-marks-from-a-string-in-qt
    // https://german.stackexchange.com/questions/4992/conversion-table-for-diacritics-e-g-%C3%BC-%E2%86%92-ue
    QString simplifyAccents(const QString &candidate)
    {
        static const QString diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
        static const QStringList noDiacriticLetters({ "S", "OE", "Z", "s", "oe", "z", "Y", "Y", "u", "A", "A", "A", "A", "A", "A", "AE", "C", "E", "E", "E", "E", "I", "I", "I", "I", "D", "N", "O", "O", "O", "O", "O", "O", "U", "U", "U", "U", "Y", "s", "a", "a", "a", "a", "a", "a", "ae", "c", "e", "e", "e", "e", "i", "i", "i", "i", "o", "n", "o", "o", "o", "o", "o", "o", "u", "u", "u", "u", "y", "y" });

        QString output = "";
        for (int i = 0; i < candidate.length(); i++)
        {
            const QChar c = candidate[i];
            int dIndex = diacriticLetters.indexOf(c);
            if (dIndex < 0)
            {
                output.append(c);
            }
            else
            {
                const QString replacement = noDiacriticLetters[dIndex];
                output.append(replacement);
            }
        }
        return output;
    }

    QString simplifyByDecomposition(const QString &s)
    {
        QString result;
        // QChar c (NOT QChar &c), see https://discordapp.com/channels/539048679160676382/539925070550794240/686321311076581440
        for (const QChar &c : s)
        {
            if (c.decompositionTag() == QChar::NoDecomposition)
            {
                result.push_back(c);
            }
            else
            {
                for (const QChar &dc : c.decomposition())
                {
                    if (!dc.isMark()) { result.push_back(dc); }
                }
            }
        }
        return result;
    }

    bool caseInsensitiveStringCompare(const QString &c1, const QString &c2)
    {
        return c1.length() == c2.length() && c1.startsWith(c2, Qt::CaseInsensitive);
    }

    QString simplifyNameForSearch(const QString &name)
    {
        return removeChars(name.toUpper(), [](QChar c) { return !c.isUpper(); });
    }

    QDateTime fromStringUtc(const QString &dateTimeString, const QString &format)
    {
        if (dateTimeString.isEmpty() || format.isEmpty()) { return QDateTime(); }
        QDateTime dt = QDateTime::fromString(dateTimeString, format);
        if (!dt.isValid()) { return dt; }
        dt.setOffsetFromUtc(0); // must only be applied to valid timestamps
        return dt;
    }

    QDateTime fromStringUtc(const QString &dateTimeString, Qt::DateFormat format)
    {
        if (dateTimeString.isEmpty()) { return QDateTime(); }
        QDateTime dt = QDateTime::fromString(dateTimeString, format);
        if (!dt.isValid()) { return dt; }
        dt.setOffsetFromUtc(0); // must only be applied to valid timestamps
        return dt;
    }

    QDateTime fromStringUtc(const QString &dateTimeString, const QLocale &locale, QLocale::FormatType format)
    {
        if (dateTimeString.isEmpty()) { return QDateTime(); }
        QDateTime dt = locale.toDateTime(dateTimeString, format);
        if (!dt.isValid()) { return dt; }
        dt.setOffsetFromUtc(0); // must only be applied to valid timestamps
        return dt;
    }

    QDateTime parseMultipleDateTimeFormats(const QString &dateTimeString)
    {
        if (dateTimeString.isEmpty()) { return QDateTime(); }
        if (isDigitsOnlyString(dateTimeString))
        {
            // 2017 0301 124421 321
            if (dateTimeString.length() == 17) { return fromStringUtc(dateTimeString, "yyyyMMddHHmmsszzz"); }
            if (dateTimeString.length() == 14) { return fromStringUtc(dateTimeString, "yyyyMMddHHmmss"); }
            if (dateTimeString.length() == 12) { return fromStringUtc(dateTimeString, "yyyyMMddHHmm"); }
            if (dateTimeString.length() == 8) { return fromStringUtc(dateTimeString, "yyyyMMdd"); }
            return QDateTime();
        }

        // remove simple separators and check if digits only again
        const QString simpleSeparatorsRemoved = removeDateTimeSeparators(dateTimeString);
        if (isDigitsOnlyString(simpleSeparatorsRemoved))
        {
            return parseMultipleDateTimeFormats(simpleSeparatorsRemoved);
        }

        // stupid trial and error
        QDateTime ts = fromStringUtc(dateTimeString, Qt::ISODateWithMs);
        if (ts.isValid()) return ts;

        ts = fromStringUtc(dateTimeString, Qt::ISODate);
        if (ts.isValid()) return ts;

        ts = fromStringUtc(dateTimeString, Qt::TextDate);
        if (ts.isValid()) return ts;

        ts = fromStringUtc(dateTimeString, QLocale(), QLocale::LongFormat);
        if (ts.isValid()) return ts;

        ts = fromStringUtc(dateTimeString, QLocale(), QLocale::ShortFormat);
        if (ts.isValid()) return ts;

        // SystemLocaleShortDate,
        // SystemLocaleLongDate,
        return QDateTime();
    }

    QDateTime parseDateTimeStringOptimized(const QString &dateTimeString)
    {
        if (dateTimeString.length() < 8) { return QDateTime(); }

        // yyyyMMddHHmmsszzz
        // 01234567890123456
        int year(dateTimeString.leftRef(4).toInt());
        int month(dateTimeString.midRef(4, 2).toInt());
        int day(dateTimeString.midRef(6, 2).toInt());
        QDate date;
        date.setDate(year, month, day);
        QDateTime dt;
        dt.setOffsetFromUtc(0);
        dt.setDate(date);
        if (dateTimeString.length() < 12) { return dt; }

        QTime t;
        const int hour(dateTimeString.midRef(8, 2).toInt());
        const int minute(dateTimeString.midRef(10, 2).toInt());
        const int second(dateTimeString.length() < 14 ? 0 : dateTimeString.midRef(12, 2).toInt());
        const int ms(dateTimeString.length() < 17 ? 0 : dateTimeString.rightRef(3).toInt());

        t.setHMS(hour, minute, second, ms);
        dt.setTime(t);
        return dt;
    }

    QString dotToLocaleDecimalPoint(QString &input)
    {
        return input.replace('.', QLocale::system().decimalPoint());
    }

    QString dotToLocaleDecimalPoint(const QString &input)
    {
        QString copy(input);
        return copy.replace('.', QLocale::system().decimalPoint());
    }

    bool stringCompare(const QString &c1, const QString &c2, Qt::CaseSensitivity cs)
    {
        if (cs == Qt::CaseSensitive) { return c1 == c2; }
        return caseInsensitiveStringCompare(c1, c2);
    }

    QString inApostrophes(const QString &in, bool ignoreEmpty)
    {
        if (in.isEmpty()) { return ignoreEmpty ? QString() : QStringLiteral("''"); }
        return u'\'' % in % u'\'';
    }

    QString inQuotes(const QString &in, bool ignoreEmpty)
    {
        if (in.isEmpty()) { return ignoreEmpty ? QString() : QStringLiteral("\"\""); }
        return u'"' % in % u'"';
    }

    QString withQuestionMark(const QString &question)
    {
        if (question.endsWith("?")) { return question; }
        return question % u'?';
    }

    int nthIndexOf(const QString &string, QChar ch, int nth, Qt::CaseSensitivity cs)
    {
        if (nth < 1 || string.isEmpty() || nth > string.length()) { return -1; }

        int from = 0;
        int ci = -1;
        for (int t = 0; t < nth; ++t)
        {
            ci = string.indexOf(ch, from, cs);
            if (ci < 0) { return -1; }
            from = ci + 1;
            if (from >= string.length()) { return -1; }
        }
        return ci;
    }

    QString joinStringSet(const QSet<QString> &set, const QString &separator)
    {
        if (set.isEmpty()) { return {}; }
        if (set.size() == 1) { return *set.begin(); }
        return set.values().join(separator);
    }

    QMap<QString, QString> parseIniValues(const QString &data)
    {
        QMap<QString, QString> map;
        QList<QStringRef> lines = splitLinesRefs(data);
        for (const QStringRef &l : lines)
        {
            if (l.isEmpty()) { continue; }
            const int i = l.indexOf("=");
            if (i < 0 || i >= l.length() + 1) { continue; }

            const QString key = l.left(i).trimmed().toString();
            const QString value = l.mid(i + 1).toString();
            if (value.isEmpty()) { continue; }
            map.insert(key, value);
        }
        return map;
    }

    QString removeSurroundingApostrophes(const QString &in)
    {
        if (in.size() < 2) { return in; }
        if (in.startsWith("'") && in.endsWith("'"))
        {
            return in.mid(1, in.length() - 2);
        }
        return in;
    }

    QString removeSurroundingQuotes(const QString &in)
    {
        if (in.size() < 2) { return in; }
        if (in.startsWith("\"") && in.endsWith("\""))
        {
            return in.mid(1, in.length() - 2);
        }
        return in;
    }

    QString removeComments(const QString &in, bool removeSlashStar, bool removeDoubleSlash)
    {
        QString copy(in);

        thread_local const QRegularExpression re1("\\/\\*(.|\\n)*?\\*\\/");
        if (removeSlashStar) { copy.remove(re1); }

        thread_local const QRegularExpression re2("\\/\\/.*");
        if (removeDoubleSlash) { copy.remove(re2); }

        return copy;
    }

    const QString &defaultIfEmpty(const QString &candidate, const QString &defaultIfEmpty)
    {
        if (candidate.isEmpty()) { return defaultIfEmpty; }
        return candidate;
    }

    bool containsAny(const QString &testString, const QStringList &any, Qt::CaseSensitivity cs)
    {
        if (testString.isEmpty() || any.isEmpty()) { return false; }
        for (const QString &a : any)
        {
            if (testString.contains(a, cs)) { return true; }
        }
        return false;
    }

    bool hasBalancedQuotes(const QString &in, char quote)
    {
        if (in.isEmpty()) { return true; }
        const int c = in.count(quote);
        return (c % 2) == 0;
    }

    double parseFraction(const QString &fraction, double failDefault)
    {
        if (fraction.isEmpty()) { return failDefault; }
        bool ok;

        double r = failDefault;
        if (fraction.contains('/'))
        {
            const QStringList parts = fraction.split('/');
            if (parts.size() != 2) { return failDefault; }
            const double c = parts.front().trimmed().toDouble(&ok);
            if (!ok) { return failDefault; }

            const double d = parts.last().trimmed().toDouble(&ok);
            if (!ok) { return failDefault; }
            if (qFuzzyCompare(0.0, d)) { return failDefault; }
            r = c / d;
        }
        else
        {
            r = fraction.trimmed().toDouble(&ok);
            if (!ok) { return failDefault; }
        }
        return r;
    }

    QString cleanNumber(const QString &number)
    {
        QString n = number.trimmed();
        if (n.isEmpty()) { return QString(); }

        int dp = n.indexOf('.');
        if (dp < 0) { dp = n.indexOf(','); }

        // clean all trailing stuff
        while (dp >= 0 && !n.isEmpty())
        {
            const QChar l = n.at(n.size() - 1);
            if (l == '0')
            {
                n.chop(1);
                continue;
            }
            else if (l == '.' || l == ',')
            {
                n.chop(1);
            }
            break;
        }

        while (n.startsWith("00"))
        {
            n.remove(0, 1);
        }

        return n;
    }

} // ns

//! \endcond
