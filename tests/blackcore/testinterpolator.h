/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORETEST_TESTAVIATIONBASE_H
#define BLACKCORETEST_TESTAVIATIONBASE_H

#include <QtTest/QtTest>

namespace BlackCoreTest
{

/*!
 * \brief Interpolator classes basic tests
 */
class CTestInterpolator : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestInterpolator(QObject *parent = 0) : QObject(parent) {}

private slots:

    /*!
     * \brief Basic unit tests for interpolator
     */
    void interpolatorBasics();
};

} // namespace

#endif // guard
