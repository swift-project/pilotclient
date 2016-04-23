/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTLIBRARYPATH_H
#define BLACKMISCTEST_TESTLIBRARYPATH_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QtTest/QtTest>

namespace BlackMiscTest
{

    //! Geo classes tests
    class CTestLibraryPath : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestLibraryPath(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for library path
        void libraryPath();
    };

} // namespace

//! \endcond

#endif // guard
