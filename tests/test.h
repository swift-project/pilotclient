// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISCTEST_TEST_H
#define SWIFT_MISCTEST_TEST_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QCoreApplication>
#include <QStringList>
#include <QTest>

//! Implements a main() function that executes all tests in TestObject
//! without instantiating a QApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define SWIFTTEST_INIT(TestObject)                                                                                     \
    TestObject to;                                                                                                     \
    QTEST_SET_MAIN_SOURCE_PATH                                                                                         \
                                                                                                                       \
    QStringList args;                                                                                                  \
    args.reserve(argc);                                                                                                \
    for (int i = 0; i < argc; ++i) { args.append(argv[i]); }                                                           \
                                                                                                                       \
    /* Output to stdout */                                                                                             \
    args.append({ "-o", "-,txt" });                                                                                    \
                                                                                                                       \
    /* Output to file */                                                                                               \
    QString resultsFileName = QString(#TestObject).replace("::", "_").toLower();                                       \
    args.append({ "-o", resultsFileName + "_testresults.xml,xml" });

//! Implements a main() function that executes all tests in TestObject
//! without instantiating a QApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define SWIFTTEST_APPLESS_MAIN(TestObject)                                                                             \
    int main(int argc, char *argv[])                                                                                   \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            SWIFTTEST_INIT(TestObject)                                                                                 \
            return QTest::qExec(&to, args);                                                                            \
        }                                                                                                              \
        catch (...)                                                                                                    \
        {                                                                                                              \
            return EXIT_FAILURE;                                                                                       \
        }                                                                                                              \
    }                                                                                                                  \
    static_assert(true)

//! Implements a main() function that executes all tests in TestObject
//! including instantiating a QCoreApplication object.
//! Also adds arguments automatically to print test results to an xml file.
#define SWIFTTEST_MAIN(TestObject)                                                                                     \
    int main(int argc, char *argv[])                                                                                   \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            QCoreApplication app(argc, argv);                                                                          \
            SWIFTTEST_INIT(TestObject)                                                                                 \
            return QTest::qExec(&to, args);                                                                            \
        }                                                                                                              \
        catch (...)                                                                                                    \
        {                                                                                                              \
            return EXIT_FAILURE;                                                                                       \
        }                                                                                                              \
    }                                                                                                                  \
    static_assert(true)

//! \endcond

#endif // guard
