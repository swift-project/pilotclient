/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*! \file */

#ifndef BLACKMISCTEST_TESTCONTAINERS_H
#define BLACKMISCTEST_TESTCONTAINERS_H

#include <QtTest/QtTest>

namespace BlackMiscTest
{

    class CTestContainers : public QObject
    {
        Q_OBJECT

    public:
        explicit CTestContainers(QObject *parent = 0) : QObject(parent) {}

    private slots:
        void collectionBasics();
        void sequenceBasics();
        void joinAndSplit();
        void findTests();
    };

} //namespace BlackMiscTest

#endif // guard
