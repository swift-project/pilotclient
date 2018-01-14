/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "stringutils.h"
#include <QChar>
#include <QTextCodec>
#include <QRegularExpression>

namespace BlackMisc
{
    QList<QStringRef> splitLinesRefs(const QString &s)
    {
        return splitStringRefs(s, [](QChar c) { return c == '\n' || c == '\r'; });
    }

    QStringList splitLines(const QString &s)
    {
        return splitString(s, [](QChar c) { return c == '\n' || c == '\r'; });
    }

    QString boolToOnOff(bool v, bool i18n)
    {
        Q_UNUSED(i18n);
        return v ? "on" : "off";
    }

    QString boolToYesNo(bool v, bool i18n)
    {
        Q_UNUSED(i18n);
        return v ? "yes" : "no";
    }

    QString boolToTrueFalse(bool v, bool i18n)
    {
        Q_UNUSED(i18n);
        return v ? "true" : "false";
    }

    QString boolToEnabledDisabled(bool v, bool i18n)
    {
        Q_UNUSED(i18n);
        return v ? "enabled" : "disabled";
    }

    bool stringToBool(const QString &string)
    {
        QString s(string.trimmed().toLower());
        if (s.isEmpty()) { return false; }

        // 1 char values
        const QChar c = s.at(0);
        if (c == '1' || c == 't' || c == 'y' || c == 'x') { return true; }
        if (c == '0' || c == 'f' || c == 'n' || c == '_') { return false; }

        // full words
        if (s == "on") { return true; }
        return false;
    }

    int fuzzyShortStringComparision(const QString &str1, const QString &str2, Qt::CaseSensitivity cs)
    {
        // same
        if (cs == Qt::CaseInsensitive) { if (caseInsensitiveStringCompare(str1, str2)) { return 100; }}
        else if (str1 == str2) { return 100; }

        // one string is empty
        if (str1.isEmpty() || str2.isEmpty()) { return 0; }

        // make sure aStr is not shorter
        const QString aStr = str1.length() >= str2.length() ? str1 : str2;
        const QString bStr = str1.length() >= str2.length() ? str2 : str1;

        // starts/ends with
        float s1 = aStr.length();
        float s2 = bStr.length();
        if (aStr.endsWith(bStr, cs)) { return s1 / s2 * 100; }
        if (aStr.startsWith(bStr, cs)) { return s1 / s2 * 100; }

        // contains
        if (aStr.contains(bStr, cs)) { return s1 / s2 * 100; }

        // char by char
        float points = 0;
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
        return points / s1 * 100;
    }

    QString intToHex(int value, int digits)
    {
        QString hex(QString::number(value, 16).toUpper());
        int l = hex.length();
        if (l >= digits) { return hex.right(digits); }
        int d = digits - l;
        return QString(d, '0') + hex;
    }

    QString bytesToHexString(const QByteArray &bytes)
    {
        QString h;
        for (int i = 0; i < bytes.size(); i++)
        {
            int b = static_cast<int>(bytes.at(i));
            h.append(intToHex(b, 2));
        }
        return h;
    }

    QByteArray byteArrayFromHexString(const QString &hexString)
    {
        QByteArray ba;
        int pos = 0;
        while (pos + 1 < hexString.length())
        {
            bool ok;
            QString h = hexString.mid(pos, 2);
            int hex = h.toInt(&ok, 16);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "Invalid hex");
            if (!ok) { return QByteArray(); }
            ba.push_back(static_cast<char>(hex));
            pos += 2;
        }
        return ba;
    }

    QString stripDesignatorFromCompleterString(const QString &candidate)
    {
        const QString s(candidate.trimmed().toUpper());
        if (s.isEmpty()) { return QString(); }
        if (s.contains(' '))
        {
            return s.left(s.indexOf(' '));
        }
        else
        {
            return s;
        }
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
    QString removeAccents(const QString &candidate)
    {
        static const QString diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
        static const QStringList noDiacriticLetters({"S", "OE", "Z", "s", "oe", "z", "Y", "Y", "u", "A", "A", "A", "A", "A", "A", "AE", "C", "E", "E", "E", "E", "I", "I", "I", "I", "D", "N", "O", "O", "O", "O", "O", "O", "U", "U", "U", "U", "Y", "s", "a", "a", "a", "a", "a", "a", "ae", "c", "e", "e", "e", "e", "i", "i", "i", "i", "o", "n", "o", "o", "o", "o", "o", "o", "u", "u", "u", "u", "y", "y"});

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

    bool caseInsensitiveStringCompare(const QString &c1, const QString &c2)
    {
        return c1.length() == c2.length() && c1.startsWith(c2, Qt::CaseInsensitive);
    }

    QString simplifyNameForSearch(const QString &name)
    {
        return removeChars(name.toUpper(), [](QChar c) { return !c.isUpper(); });
    }

    QDateTime parseMultipleDateTimeFormats(const QString &dateTimeString)
    {
        if (dateTimeString.isEmpty()) { return QDateTime(); }
        if (isDigitsOnlyString(dateTimeString))
        {
            // 2017 0301 124421 321
            if (dateTimeString.length() == 17)
            {
                return QDateTime::fromString(dateTimeString, "yyyyMMddHHmmsszzz");
            }
            if (dateTimeString.length() == 14)
            {
                return QDateTime::fromString(dateTimeString, "yyyyMMddHHmmss");
            }
            if (dateTimeString.length() == 12)
            {
                return QDateTime::fromString(dateTimeString, "yyyyMMddHHmm");
            }
            if (dateTimeString.length() == 8)
            {
                return QDateTime::fromString(dateTimeString, "yyyyMMdd");
            }
            return QDateTime();
        }

        // remove simple separators and check if digits only again
        const QString simpleSeparatorsRemoved = removeChars(dateTimeString, [](QChar c)
        {
            return c == ' ' || c == ':' || c == '_' || c == '-';
        });
        if (isDigitsOnlyString(simpleSeparatorsRemoved))
        {
            return parseMultipleDateTimeFormats(simpleSeparatorsRemoved);
        }

        // stupid trial and error
        QDateTime ts = QDateTime::fromString(dateTimeString, Qt::ISODateWithMs);
        if (ts.isValid()) return ts;

        ts = QDateTime::fromString(dateTimeString, Qt::ISODate);
        if (ts.isValid()) return ts;

        ts = QDateTime::fromString(dateTimeString, Qt::TextDate);
        if (ts.isValid()) return ts;

        ts = QDateTime::fromString(dateTimeString, Qt::DefaultLocaleLongDate);
        if (ts.isValid()) return ts;

        ts = QDateTime::fromString(dateTimeString, Qt::DefaultLocaleShortDate);
        if (ts.isValid()) return ts;

        // SystemLocaleShortDate,
        // SystemLocaleLongDate,
        return QDateTime();
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
}

//! \endcond
