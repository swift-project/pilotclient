/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORETEST_TESTAVIATIONBASE_H
#define BLACKCORETEST_TESTAVIATIONBASE_H

#include <QtTest/QtTest>

namespace BlackCoreTest
{

    //! Interpolator classes basic tests
    class CTestInterpolator : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestInterpolator(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for interpolator
        void linearInterpolator();
    };

} // namespace

#endif // guard
