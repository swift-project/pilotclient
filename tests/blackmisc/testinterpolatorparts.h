/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTINTERPOLATORPARTS_H
#define BLACKMISCTEST_TESTINTERPOLATORPARTS_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/aviation/aircraftparts.h"

#include <QObject>
#include <QtGlobal>

namespace BlackMiscTest
{
    /*!
     * Aircraft parts interpolation, mainly ground flag interpolation
     */
    class CTestInterpolatorParts : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestInterpolatorParts(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for interpolator
        void groundFlagInterpolation();

    private:
        //! Test parts
        static BlackMisc::Aviation::CAircraftParts getTestParts(int number, qint64 ts, qint64 deltaT, bool onGround);
    };
} // namespace

//! \endcond

#endif // guard
