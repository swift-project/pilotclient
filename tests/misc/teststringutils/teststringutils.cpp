// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QTest>
#include <QTextCodec>
#include <QTime>

#include "test.h"

#include "misc/stringutils.h"

using namespace swift::misc;

namespace MiscTest
{
    //! Testing string utilities
    class CTestStringUtils : public QObject
    {
        Q_OBJECT

    private slots:
        void testRemove();
        void testContains();
        void testIndexOf();
        void testSplit();
        void testTimestampParsing();
        void testCodecs();
        void testSimplify();
    };

    void CTestStringUtils::testRemove()
    {
        QString s = "loUwP PeERr69";
        QVERIFY2(removeChars(s, [](QChar c) { return !c.isUpper(); }) == "UPPER", "Test removing characters by predicate");
    }

    void CTestStringUtils::testContains()
    {
        QString s = "string with a numb3r";
        QVERIFY2(containsChar(s, [](QChar c) { return c.isNumber(); }), "Test contains character by predicate");
        s = "string without a number";
        QVERIFY2(!containsChar(s, [](QChar c) { return c.isNumber(); }), "Test not contains character by predicate");
    }

    void CTestStringUtils::testIndexOf()
    {
        QString s = "string with a numb3r";
        QVERIFY2(indexOfChar(s, [](QChar c) { return c.isNumber(); }) == 18, "Test index of character by predicate");
        s = "string without a number";
        QVERIFY2(indexOfChar(s, [](QChar c) { return c.isNumber(); }) == -1, "Test not index of character by predicate");
    }

    void CTestStringUtils::testSplit()
    {
        const QString s = "line one\nline two\r\nline three\n";
        QStringList lines = splitLines(s);
        QVERIFY2(lines.size() == 3, "Test split string into lines: correct number of lines");
        QVERIFY2(lines[0] == "line one", "Test split string into lines: correct first line");
        QVERIFY2(lines[1] == "line two", "Test split string into lines: correct second line");
        QVERIFY2(lines[2] == "line three", "Test split string into lines: correct third line");
    }

    void CTestStringUtils::testTimestampParsing()
    {
        const QStringList dts(
            { "2018-01-01 11:11:11",
              "2012-05-09 03:04:05.777",
              "2012-05-09 00:00:00.000",
              "2015-12-31 03:04:05",
              "1999-12-31 23:59:59.999",
              "1975-01-01 14:13:17",
              "1982-05-09 03:01:05.123",
              "2000-05-02 00:04:00.000",
              "2002-12-31 03:34:33",
              "1992-11-01 21:59:29.999" });

        const int size = QString("yyyyMMddHHmmss").size();
        for (const QString &dt : dts)
        {
            const QString c = removeDateTimeSeparators(dt);
            const QDateTime dt1 = parseDateTimeStringOptimized(c);
            const QDateTime dt2 = (c.length() == size) ?
                                      fromStringUtc(c, "yyyyMMddHHmmss") :
                                      fromStringUtc(c, "yyyyMMddHHmmsszzz");
            QDateTime dt3 = (c.length() == size) ?
                                QDateTime::fromString(c, "yyyyMMddHHmmss") :
                                QDateTime::fromString(c, "yyyyMMddHHmmsszzz");
            dt3.setOffsetFromUtc(0);

            const qint64 ms1 = dt1.toMSecsSinceEpoch();
            const qint64 ms2 = dt2.toMSecsSinceEpoch();
            const qint64 delta = ms1 - ms2;

            QVERIFY2(dt1 == dt2, "Expect same results of QDateTime");
            QVERIFY2(dt1 == dt3, "Expect same results of QDateTime");
            QVERIFY2(delta == 0, "Expect same results timestamp");
        }

        // performance
        int constexpr Loops = 10000;
        QElapsedTimer time;
        time.start();
        for (int i = 0; i < Loops; i++)
        {
            for (const QString &dt : dts)
            {
                const QString c = removeDateTimeSeparators(dt);
                const QDateTime dateTime = parseDateTimeStringOptimized(c);
                parseDateTimeStringOptimized(c);
                Q_UNUSED(dateTime); // avoid optimizing out of call
            }
        }
        const int elapsedOptimized = time.restart();

        for (int i = 0; i < Loops; i++)
        {
            for (const QString &dt : dts)
            {
                const QString c = removeDateTimeSeparators(dt);
                const QDateTime dateTime = (c.length() == size) ?
                                               fromStringUtc(c, "yyyyMMddHHmmss") :
                                               fromStringUtc(c, "yyyyMMddHHmmsszzz");
                Q_UNUSED(dateTime); // avoid optimizing out of call
            }
        }
        const int elapsedQt = time.restart();

        qDebug() << "Parsing date/time, optimized" << elapsedOptimized << "vs. QDateTime: " << elapsedQt;
        QVERIFY2(elapsedOptimized < elapsedQt, "Expect optimized being faster as QDateTim::fromString");
    }

    void CTestStringUtils::testCodecs()
    {
        QTextCodec *latin1 = QTextCodec::codecForName("latin1");
        QTextCodec *cp1251 = QTextCodec::codecForName("windows-1251");
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        const QString testEnglish = QStringLiteral(u"test");
        const QString testRussian = QStringLiteral(u"тест");
        bool okEn1 = latin1->toUnicode(latin1->fromUnicode(testEnglish)) == testEnglish;
        bool okEn2 = utf8->toUnicode(utf8->fromUnicode(testEnglish)) == testEnglish;
        bool okRu1 = cp1251->toUnicode(cp1251->fromUnicode(testRussian)) == testRussian;
        bool okRu2 = utf8->toUnicode(utf8->fromUnicode(testRussian)) == testRussian;
        QVERIFY2(okEn1, "English \"test\" equal after round-trip with latin1");
        QVERIFY2(okEn2, "English \"test\" equal after round-trip with utf8");
        QVERIFY2(okRu1, "Russian \"test\" equal after round-trip with cp1251");
        QVERIFY2(okRu2, "Russian \"test\" equal after round-trip with utf8");
    }

    void CTestStringUtils::testSimplify()
    {
        const auto inputChars = u8"ŠŽšžŸÀÁÂÃÄÅÇÈÉÊËÌÍÎÏÑÒÓÔÕÖÙÚÛÜÝàáâãäåçèéêëìíîïñòóôõöùúûüýÿ";
        const auto outputChars = "SZszYAAAAAACEEEEIIIINOOOOOUUUUYaaaaaaceeeeiiiinooooouuuuyy";
        const QString input = QString::fromUtf8(reinterpret_cast<const char *>(inputChars));
        const QString output = QLatin1String(outputChars);
        QCOMPARE(simplifyAccents(input), output);
        QCOMPARE(simplifyByDecomposition(input), output);
    }
} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestStringUtils);

#include "teststringutils.moc"

//! \endcond
