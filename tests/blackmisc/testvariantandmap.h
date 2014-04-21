/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTVARIANTANDMAP_H
#define BLACKMISCTEST_TESTVARIANTANDMAP_H

#include "blackmisc/pqconstants.h"
#include "blackmisc/avatcstation.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{

    /*!
     * \brief Variant and map related tests
     */
    class CTestVariantAndMap : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestVariantAndMap(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for value objects and variants
        void variant();
        //! Unit tests for value maps and value objects
        void valueMap();
    };

} // namespace

#endif // guard
