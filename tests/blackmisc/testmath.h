/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_TESTMATH_H
#define BLACKMISCTEST_TESTMATH_H

#include <QtTest/QtTest>

namespace BlackMiscTest
{

    //! Math classes tests
    class CTestMath : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestMath(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Unit test for round to multiple of
        void testRoundToMultipleOf();
    };

} // namespace

#endif // guard
