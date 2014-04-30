/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H
#define BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H

#include <QtTest/QtTest>
#include "blackmisc/pqconstants.h"

namespace BlackMiscTest
{

    /*!
     * \brief Physical quantities, basic tests
     */
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

        //! Testing angles
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

#endif // guard
