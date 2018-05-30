/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTINTERPOLATORMISC_H
#define BLACKMISCTEST_TESTINTERPOLATORMISC_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>
#include <QtGlobal>

namespace BlackMiscTest
{
    /*!
     * Interpolator related basic tests
     */
    class CTestInterpolatorMisc : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestInterpolatorMisc(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for interpolation setup
        void setupTests();
    };
} // namespace

//! \endcond

#endif // guard
