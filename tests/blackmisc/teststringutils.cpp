/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "teststringutils.h"
#include "blackmisc/stringutils.h"

#include <QTest>

using namespace BlackMisc;

namespace BlackMiscTest
{

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
        QString s = "line one\nline two\r\nline three\n";
        QStringList lines = splitLines(s);
        QVERIFY2(lines.size() == 3, "Test split string into lines: correct number of lines");
        QVERIFY2(lines[0] == "line one", "Test split string into lines: correct first line");
        QVERIFY2(lines[1] == "line two", "Test split string into lines: correct second line");
        QVERIFY2(lines[2] == "line three", "Test split string into lines: correct third line");
    }

}

//! \endcond
