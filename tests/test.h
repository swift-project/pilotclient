/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TEST_H
#define BLACKMISCTEST_TEST_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include <QTest>
#include <QStringList>
#include <QCoreApplication>

//! Implements a main() function that executes all tests in TestObject
//! without instantiating a QApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define BLACKTEST_INIT(TestObject)                                               \
    TestObject to;                                                               \
    QTEST_SET_MAIN_SOURCE_PATH                                                   \
                                                                                 \
    QStringList args;                                                            \
    args.reserve(argc);                                                          \
    for (int i = 0; i < argc; ++i)                                               \
    {                                                                            \
        args.append(argv[i]);                                                    \
    }                                                                            \
                                                                                 \
    /* Output to stdout */                                                       \
    args.append({ "-o", "-,txt" });                                              \
                                                                                 \
    /* Output to file */                                                         \
    QString resultsFileName = QString(#TestObject).replace("::", "_").toLower(); \
    args.append({ "-o", resultsFileName + "_testresults.xml,xml" });

//! Implements a main() function that executes all tests in TestObject
//! without instantiating a QApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define BLACKTEST_APPLESS_MAIN(TestObject)  \
    int main(int argc, char *argv[])        \
    {                                       \
        try                                 \
        {                                   \
            BLACKTEST_INIT(TestObject)      \
            return QTest::qExec(&to, args); \
        }                                   \
        catch (...)                         \
        {                                   \
            return EXIT_FAILURE;            \
        }                                   \
    }

//! Implements a main() function that executes all tests in TestObject
//! including instantiating a QCoreApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define BLACKTEST_MAIN(TestObject)            \
    int main(int argc, char *argv[])          \
    {                                         \
        try                                   \
        {                                     \
            QCoreApplication app(argc, argv); \
            BLACKTEST_INIT(TestObject)        \
            return QTest::qExec(&to, args);   \
        }                                     \
        catch (...)                           \
        {                                     \
            return EXIT_FAILURE;              \
        }                                     \
    }

//! \endcond

#endif // guard
