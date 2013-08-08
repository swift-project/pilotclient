/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORETEST_TESTNETMEDIATORS_H
#define BLACKCORETEST_TESTNETMEDIATORS_H

#include "blackcore/atclistmgr.h"
#include "blackcoretest.h"
#include <QtTest/QtTest>

namespace BlackCoreTest
{

    /*!
     * Network mediator classes tests
     */
    class CTestNetMediators : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param parent
         */
        explicit CTestNetMediators(QObject *parent = 0) : QObject(parent) {}

    private slots:
        /*!
         * Test CAtcListManager
         */
        void atcListManagerTest();
    };

    /*!
     * Helper class that connects to CAtcListManager signals
     */
    class AtcListConsumer : public QObject
    {
        Q_OBJECT

    public:
        //! List is updated by the slot
        BlackMisc::CAtcList m_list;

    public slots:
        /*!
         * Slot updates the list
         * \param list
         */
        void listChanged(const BlackMisc::CAtcList &list)
        {
            m_list = list;
        }
    };

} //namespace BlackCoreTest

#endif //BLACKCORETEST_TESTNETMEDIATORS_H