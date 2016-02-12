/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTINTERPOLATOR_H
#define BLACKCORETEST_TESTINTERPOLATOR_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 */

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftparts.h"

#include <QtTest/QtTest>

namespace BlackCoreTest
{

    /*!
     * Interpolator classes basic tests
     */
    class CTestInterpolator : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestInterpolator(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for interpolator
        void linearInterpolator();

    private:
        //! Test situation for testing
        static BlackMisc::Aviation::CAircraftSituation getTestSituation(const BlackMisc::Aviation::CCallsign &callsign, int number, qint64 ts, qint64 deltaT);

        //! Test parts
        static BlackMisc::Aviation::CAircraftParts getTestParts(int number, qint64 ts, qint64 deltaT);

    };

} // namespace

//! \endcond

#endif // guard
