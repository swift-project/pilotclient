/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORETEST_TESTNETWORK_H
#define BLACKCORETEST_TESTNETWORK_H

#include "blackcore/network_vatlib.h"
#include <QtTest/QtTest>

namespace BlackCoreTest
{

    /*!
     * INetwork implementation classes tests
     */
    class CTestNetwork : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param parent
         */
        explicit CTestNetwork(QObject *parent = 0) : QObject(parent) {}

    private slots:
        /*!
         * Test NetworkVatlib
         */
        void networkVatlibTest() { networkTest(&m_networkVatlib); }

    private:
        /*!
         * Common part used by all tests.
         */
        void networkTest(BlackCore::INetwork *);

        BlackCore::NetworkVatlib m_networkVatlib;
    };

} //namespace BlackCoreTest

#endif //BLACKCORETEST_TESTNETWORK_H