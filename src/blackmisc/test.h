/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_H
#define BLACKMISC_TEST_H

#include "blackmisc/blackmiscexport.h"
#include <QStringList>

#ifdef QT_TESTLIB_LIB
#include <QtTest>
#endif

class QObject;

namespace BlackMisc
{
    /*!
     * Helper base class for executing unit tests.
     * Contains the parts that do not depend upon QtTest library, and pure
     * virtual functions for derived class to implement the parts that do.
     */
    class BLACKMISC_EXPORT CTestBase
    {
    public:
        //! Constructor expects command line arguments.
        CTestBase(int argc, const char *const *argv);

        //! Destructor.
        virtual ~CTestBase();

        //! Execute the given test case.
        int exec(QObject *test, const QString &name);

    private:
        virtual int qExec(QObject *test, const QStringList &args) = 0;

        QStringList m_arguments;
    };

#ifdef QT_TESTLIB_LIB
    /*!
     * Helper derived class for executing unit tests.
     * Defined inline and only when building with QT += testlib so blackmisc
     * doesn't need to link against QtTest library.
     */
    class CTest : public CTestBase
    {
    public:
        using CTestBase::CTestBase;

    private:
        virtual int qExec(QObject *test, const QStringList &args) override { return QTest::qExec(test, args); }
    };
#endif
}

#endif
