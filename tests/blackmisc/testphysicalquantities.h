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
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestPhysicalQuantities(QObject *parent = nullptr) : QObject(parent) {}

private slots:
    /*!
     * \brief Basic unit tests for physical units
     */
    void unitsBasics();

    /*!
     * \brief Basic tests around length
     */
    void lengthBasics();

    /*!
     * \brief Basic tests about speed
     */
    void speedBasics();

    /*!
     * \brief Frequency tests
     */
    void frequencyTests();

    /*!
     * \brief Testing angles (degrees / radians)
     */
    void angleTests();

    /*!
     * \brief Testing angles
     */
    void massTests();

    /*!
     * \brief Testing pressure
     */
    void pressureTests();

    /*!
     * \brief Testing temperature
     */
    void temperatureTests();

    /*!
     * \brief Testing time
     */
    void timeTests();

    /*!
     * \brief Testing acceleration
     */
    void accelerationTests();

    /*!
     * \brief Testing construction / destruction in memory
     */
    void memoryTests();

    /*!
     * \brief Basic arithmetic such as +/-
     */
    void basicArithmetic();

};

} // namespace

#endif // guard
