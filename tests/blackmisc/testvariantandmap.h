/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTVARIANTANDVALUEMAP_H
#define BLACKMISCTEST_TESTVARIANTANDVALUEMAP_H

#include "blackmisc/pqconstants.h"
#include "blackmisc/avatcstation.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{

    /*!
     * \brief Aviation classes basic tests
     */
    class CTestVariantAndValueMap : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief Standard test case constructor
         * \param parent
         */
        explicit CTestVariantAndValueMap(QObject *parent = 0) : QObject(parent) {}

    private slots:
        /*!
         * \brief Basic unit tests for value objects and variants
         */
        void variant();
        /*!
         * \brief Unit tests for value maps and value objects
         */
        void valueMap();

    };

} // namespace

#endif // guard
