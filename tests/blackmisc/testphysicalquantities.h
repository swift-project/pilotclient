/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H
#define BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>

namespace BlackMiscTest
{
    //! Physical quantities, basic tests
    class CTestPhysicalQuantities : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestPhysicalQuantities(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for physical units
        void unitsBasics();

        //! Basic tests around length
        void lengthBasics();

        //! Basic tests about speed
        void speedBasics();

        //! Frequency tests
        void frequencyTests();

        //! Testing angles (degrees / radians)
        void angleTests();

        //! Testing mass
        void massTests();

        //! Testing pressure
        void pressureTests();

        //! Testing temperature
        void temperatureTests();

        //! Testing time
        void timeTests();

        //! Testing acceleration
        void accelerationTests();

        //! Testing construction / destruction in memory
        void memoryTests();

        //! Test parsing on PQs
        void parserTests();

        //! Basic arithmetic such as +/-
        void basicArithmetic();
    };
} // namespace

//! \endcond

#endif // guard
