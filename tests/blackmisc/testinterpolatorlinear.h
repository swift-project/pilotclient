/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTINTERPOLATORLINEAR_H
#define BLACKMISCTEST_TESTINTERPOLATORLINEAR_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include <QObject>
#include <QtGlobal>

namespace BlackMisc { namespace Aviation { class CCallsign; } }
namespace BlackMiscTest
{
    /*!
     * Interpolator classes basic tests
     */
    class CTestInterpolatorLinear : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestInterpolatorLinear(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for interpolator
        void basicInterpolatorTests();

    private:
        //! Test situation for testing
        static BlackMisc::Aviation::CAircraftSituation getTestSituation(const BlackMisc::Aviation::CCallsign &callsign, int number, qint64 ts, qint64 deltaT, qint64 offset);

        //! Test parts
        static BlackMisc::Aviation::CAircraftParts getTestParts(int number, qint64 ts, qint64 deltaT);
    };
} // namespace

//! \endcond

#endif // guard
