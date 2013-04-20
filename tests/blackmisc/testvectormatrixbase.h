/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTVECTORMATRIX_H
#define BLACKMISCTEST_TESTVECTORMATRIX_H

#include "blackmisc/mathmatrix3x3.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{

/*!
 * \brief Vector and Matrix classes basic tests
 */
class CTestVectorMatrixBase : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestVectorMatrixBase(QObject *parent = 0) : QObject(parent) {}

private slots:
    /*!
     * \brief Basic unit tests for physical units
     */
    void vectorBasics();
    /*!
     * \brief Vertical positions
     */
    void matrixBasics();
};

} // namespace

#endif // guard
