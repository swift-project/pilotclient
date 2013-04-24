/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTGEO_H
#define BLACKMISCTEST_TESTGEO_H

#include "blackmisc/coordinatetransformation.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{

/*!
 * \brief Geo classes tests
 */
class CTestGeo : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestGeo(QObject *parent = 0) : QObject(parent) {}

private slots:
    /*!
     * \brief Basic unit tests for geo classes
     */
    void geoBasics();
};

} // namespace

#endif // guard
